#include "parser.hpp"

/*
File   : parser.cpp
Authors: Darren White
Team   : Team X
*/

/* struct pusher:
	A structure of this type is required by boost::xpressive to call
	functions from regular expression (regex) matching.  Pusher simply
	calls the std::vector push_back function.
*/
struct pusher{
    typedef void result_type; // Result type, needed for xpressive

    template<typename Vector, typename Value>
    void operator()(Vector &vec, Value const &val) const{
        vec.push_back(val);
    }
};
function<pusher>::type const vpush = {{}};

/* Constructor:
	Initialize string regular expressions (sregex) commonly needed for
	parsing operations.  n_temps is an integer value containing the number
	of temporary names assigned to relations.  This is to ensure that all
	relations created are unique.
*/
Parser::Parser(){
	id         = (alpha >> *(alpha | _d | '_'));
	literals   = (('"' >> +alnum >> '"') | +_d);
	op         = (as_xpr("==") | "!=" | '<' | '>' | "<=" | ">=");
	type       = (icase("VARCHAR") >> '(' >> +_d >> ')') | icase("INTEGER");
	query_op   = (as_xpr('+') | as_xpr('-') | as_xpr('*') | as_xpr("JOIN"));

	atomic_expr = 
		'(' >> +_ >> ')'
		| *_s >> id >> *_s
		| *_s >> by_ref(atomic_expr) >> *_s >> query_op >> *_s >> by_ref(atomic_expr);

	n_temps = 0; // A count of temporary names assigned
}

/* match(string &input):
	This is the entry point for the Parser module.  It establishes some regex match
	criteria, searches for operations that need to be broken up like union, difference,
	product, and natural_join, and splits around them if necessary.  If a query is
	of a temporary nature (and thus assigned no name), a name is generated for it.
	Return: A pointer to the relation created.
*/
Relation * Parser::match(string &input){
	int n = -1; // The switch statement condition
	string name, expr1, expr2, expr_op;
	Relation * r;
	Relation *r1;
	Relation *r2;

	sregex expression = (s1 = atomic_expr)[ref(expr1) = s1] >> *_s >>
		(s2 = query_op)[ref(expr_op) = s2] >> *_s >> (s3 = atomic_expr)[ref(expr2) = s3];

	sregex query = 
		( *_s >> icase("create")[ref(n) = CREATE] >> *_
	    | *_s >> icase("update")[ref(n) = UPDATE] >> *_
		| *_s >> icase("insert")[ref(n) = INSERT] >> *_
		| *_s >> icase("delete")[ref(n) = DELETE] >> *_
		| *_s >> id >> *_s >> "<-" >> *_s >> icase("select")[ref(n) = SELECT] >> *_
		| *_s >> id >> *_s >> "<-" >> *_s >> icase("project")[ref(n) = PROJECT] >> *_
		| *_s >> id >> *_s >> "<-" >> *_s >> icase("rename")[ref(n) = RENAME] >> *_
		| *_s >> *(as_xpr('(')) >> icase("select")[ref(n) = SELECT] >> *_
		| *_s >> *(as_xpr('(')) >> icase("project")[ref(n) = PROJECT] >> *_
		| *_s >> *(as_xpr('(')) >> icase("rename")[ref(n) = RENAME] >> *_
		| (*_s >> (s1 = id)[ref(name) = s1] >> *_s >> "<-" >> *_s >> expression)[ref(n) = EXPR]
		| (*(as_xpr('(')) >> expression >> *(as_xpr(')')))[ref(n) = EXPR]
		| *_s >> *(as_xpr('(')) >> *_s >> (s1 = id)[ref(name) = s1] >> *_s >> *(as_xpr(')')));

	regex_match(input, query);  // n will get assigned an operation
	switch(n){
		case CREATE :  return create_cmd(input);
		case UPDATE :  return update_cmd(input);
		case INSERT :  return insert_cmd(input);
		case DELETE :  return delete_cmd(input);
		case SELECT :  return select_query(input);
		case PROJECT:  return project_query(input);
		case RENAME :  return rename_query(input);
		case EXPR   :
			if(name.empty())
				name = temp_name();      // Generate a temporary name
			r1 = match(expr1);  // LHS side of the operation
			r2 = match(expr2);  // RHS side of the operation
			r = expr_query(name, r1, r2, expr_op);
			if(name[0] == '$')
				dbms->add_relation(r);   // Make Temp relations available to others
			return r;
		default:
			if(!name.empty())            // Input was a relation name
				return dbms->get_relation(name); 
			else
				throw Error("\n **Parser Error: invalid input\n");
	}
} // end match()

/* create_cmd(string &cmd):
	Parses and tokenizes command expressions all at once.  If the input was good, it creates
	the new relation with the provided data.  Otherwise, throws a parser error.
	FIXME: Should somehow flag it to be persistent
	Return:  Pointer to the new relation
*/
Relation * Parser::create_cmd(string &cmd){
	string name;
	vector<string> v_attr, v_types, v_keys;

	// Establish criteria for a match, while simultaneously populating vectors with data
	sregex keys = 
		('(' >> *_s >> (s1 = id)[vpush(ref(v_keys), s1)] >> *_s >> 
		*(',' >> *_s >> (s2 = id)[vpush(ref(v_keys), s2)]) >> *_s >> ')');

	sregex typed_attr_list = 
		('(' >> *_s >> (s1 = id)[vpush(ref(v_attr), s1)] >> *_s >> (s2 = type)[vpush(ref(v_types), s2)] >> *_s >>
		*(',' >> *_s >> (s3 = id)[vpush(ref(v_attr), s3)] >> *_s >> (s4 = type)[vpush(ref(v_types), s4)]) >> 
		*_s >> ')');

	sregex cmd_create = 
		(icase("CREATE TABLE") >> *_s >> (s1 = id)[ref(name) = s1] >> *_s >> typed_attr_list >> *_s >>
		icase("PRIMARY KEY") >> *_s >> keys >> *_s);

	if(regex_match(cmd, cmd_create)){
		Relation *r = new Relation(name, v_attr, v_types, v_keys);
		r->b_save = true;     // This relation needs to be saved
		return r;
	}
	else
		throw Error("\n **Parser Error: create command\n");
} // end create_cmd

/* insert_cmd(string &cmd):
	Parses and tokenizes insert commands.  On a match, pulls the the required relation from the database and
	calls insert on it.  In the FROM RELATION case, it evaluates the expr, and calls the overloaded insert.
	Throws an error if the input was bad
	Return: Pointer to the new relation
*/
Relation * Parser::insert_cmd(string &cmd){
	string name, expr;
	vector<string> v_tuple;

	sregex cmd_insert = 
		(icase("INSERT INTO") >> *_s >> (s1 = id)[ref(name) = s1] >> *_s >> icase("VALUES FROM") >> *_s >>
		'(' >> *_s >> (s2 = literals)[vpush(ref(v_tuple), s2)] >> *_s >> *(',' >> *_s >> 
		(s3 = literals)[vpush(ref(v_tuple), s3)]) >> *_s >> ')' >> *_s) | (icase("INSERT INTO") >> *_s >> 
		(s4 = id)[ref(name) = s4] >> *_s >> icase("VALUES FROM RELATION") >> *_s >> (s5 = *_)[ref(expr) = s5]);

	if(regex_match(cmd, cmd_insert)){
		Relation *r = dbms->get_relation(name); 
		if(expr.empty()){  // VALUES FROM case
			remove_quotes(v_tuple);
			r->add_tuple(v_tuple);
		}
		else{ // FROM RELATION case
			Relation *from_relation = match(expr);
			r = dbms->insert(r, from_relation);
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: insert command\n");
} // end insert_cmd

/* update_cmd(string &cmd):
	Parses and tokenizes update commands to change values in a relation.  On a match, pulls 
	the the required relation from the database and calls database::update on it.
	Will call condition to evaluate the condition portion.
	Throws an error if the input was bad
	Return: Pointer to the new relation
*/
Relation * Parser::update_cmd(string &cmd){
	string name, cond;
	vector<string> v_attr, v_literals;

	sregex cmd_update = 
		(icase("UPDATE") >> *_s >> (s1 = id)[ref(name) = s1] >> *_s >> icase("SET") >> *_s >>
		(s2 = id)[vpush(ref(v_attr), s2)] >> *_s >> "=" >> *_s >> (s3 = literals)[vpush(ref(v_literals), s3)] >>
		*_s >> *(',' >> *_s >> (s4 = id)[vpush(ref(v_attr), s4)] >> *_s >> "=" >> *_s >> 
		(s5 = literals)[vpush(ref(v_literals), s5)]) >> *_s >> icase("WHERE") >> *_s >> (s6 = *_)[ref(cond) = s6]);

	if(regex_match(cmd, cmd_update)){ // Populates vectors while matching
		Relation *r = dbms->get_relation(name);
		vector<bool> matches;
		matches = condition(r, cond);  // Matches now has index to rows needing to change.
		remove_quotes(v_literals);
		for(int i = 0; i < (int) matches.size(); i++){
			if(matches[i] == true){
				r->update_attrs(v_attr, v_literals, i);
			}
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: update command\n");
} // end update_cmd

/* delete_cmd(string &cmd):
	Parses and tokenizes delete commands.  On a match, pulls the the required relation from the database and
	calls delete on it.  Will call condition to evaluate the condition portion.
	Throws an error if the input was bad
	Return: Pointer to the new relation
*/
Relation * Parser::delete_cmd(string &cmd){
	string name, cond;

	sregex cmd_delete = 
		(icase("DELETE FROM") >> *_s >> (s1 = id)[ref(name) = s1] >> *_s >> icase("WHERE") >> *_s >> 
		(s2 = *_)[ref(cond) = s2]);

	if(regex_match(cmd, cmd_delete)){  // Populates variables while matching
		Relation *r = dbms->get_relation(name);
		vector<bool> matches;
		matches = condition(r, cond);   // Matches now has index to row we want.
		for(int i = (int) matches.size()-1; i >= 0; i--){ // Remove the appropriate rows
			if(matches[i] == true){
				r->remove_tuple(i);
			}
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: delete command\n");
} // end delete

/* select_query(string &cmd):
	This query is performed entirely within the parser (unlike the others).  Will match the input
	for a valid query, evaluate the conditions, and create a new relation.  In the case this is an
	intermediate query (and it has no name), a temporary name is generated with temp_name().
	Throws an error if the input is invalid.
	Return:  The new relation
*/
Relation * Parser::select_query(string &cmd){
	string name = "", cond, more;
	smatch what;
	
	sregex conditions = 
		skip(_s)(s1 = ('(' >> +_ >> ')'))[ref(cond) = s1];

    sregex selection = 
		(*_s >> (s1 = id)[ref(name) = s1] >> *_s >> "<-" >> *_s >> icase("select") >> conditions >> 
		*_s >> (s2 = atomic_expr)[ref(more) = s2])
		| (*(as_xpr('(')) >> *_s >> icase("select") >> conditions >> *_s >> (s1 = atomic_expr)[ref(more) = s1]
		>> *(as_xpr(')')));              

	if(regex_match(cmd, what, selection)){
		if(name.empty())
			name = temp_name();
		Relation *r = new Relation;
		Relation *from_relation = match(more);
		from_relation->copy_attrs(r);  // Copy the column names to the relation we will return
		r->name = name;
		vector<bool> matches;
		matches = condition(from_relation, cond);   // Matches now has indexes to rows we want.
		for(int i = 0; i < (int) matches.size(); i++){
			if(matches[i] == true){
				r->add_tuple(from_relation->get_tuple(i)); // Add the selected rows
			}
		}
		if(name[0] == '$'){  // If this was a temp, make it available to other operations
			dbms->add_relation(r); 
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: select_query\n");
} // end select_query

/* project_query(string &cmd):
	Establishes criteria for matching a project query, attempts to match it, and creates the new
	relation.  Will use database::project.  Throws an error if input is invalid
	Return: The new relation
*/
Relation * Parser::project_query(string &cmd){
	string name, more;
	vector<string> v_attrs;
	sregex attr_list = 
		('(' >> *_s >> (s1 = id)[vpush(ref(v_attrs), s1)] >> *_s >> *(',' >> *_s >> 
		(s2 = id)[vpush(ref(v_attrs), s2)]) >> *_s >> ')');

	sregex projection = 
		(*_s >> (s1 = id)[ref(name) = s1] >> *_s >> "<-" >> *_s >> icase("project") >> *_s >>
		attr_list >> *_s >> (s2 = atomic_expr)[ref(more) = s2])
		| (*_s >> *(as_xpr('(')) >> *_s >> icase("project") >> *_s >> attr_list >> *_s >> 
		(s2 = atomic_expr)[ref(more) = s2] >> *(as_xpr(')')));

	if(regex_match(cmd, projection)){
		if(name.empty())
			name = temp_name();
		Relation *r;
		Relation *from_relation = match(more);
		r = dbms->project(name, from_relation, v_attrs);
		if(name[0] == '$'){  // If this was a temp, make it available to other operations
			dbms->add_relation(r);
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: project_query\n");
} // end project_query

/* rename_query(string &cmd):
	Establishes criteria for matching a rename query, attempts to match it, and returns the changed relation.
	Throws an error if the input is invalid.
	Return: The new relation
*/
Relation * Parser::rename_query(string &cmd){
	string name, more;
	vector<string> v_attrs;
	sregex attr_list = 
		('(' >> *_s >> (s1 = id)[vpush(ref(v_attrs), s1)] >> *_s >> *(',' >> *_s >> (s2 = id)[vpush(ref(v_attrs), s2)]) >>
		*_s >> ')');

	sregex renaming = 
		(*_s >> (s1 = id)[ref(name) = s1] >> *_s >> "<-" >> *_s >> icase("rename") >> *_s >>
		attr_list >> *_s >> (s2 = atomic_expr)[ref(more) = s2])
		| (*_s >> *(as_xpr('(')) >> *_s >> icase("rename") >> *_s >> attr_list >> *_s >> 
		(s2 = atomic_expr)[ref(more) = s2] >> *(as_xpr(')')));

	if(regex_match(cmd, renaming)){
		if(name.empty())
			name = temp_name();
		Relation *r;
		Relation *from_relation = match(more);
		r = dbms->rename(name, from_relation, v_attrs);
		if(name[0] == '$'){  // If this was a temp, make it available to other operations
			dbms->add_relation(r);
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: rename_query\n");
} // end rename_query

/* expr_query(string &name, Relation *r1, Relation *r2, string operation):
	Calls the appropriate database operation based on the operation symbol.
	Throws an error if the operation was invalid.  Note that JOIN is case sensitive
	Return: the result sent from the database.
*/
Relation * Parser::expr_query(string &name, Relation *r1, Relation *r2, string operation){
	switch(operation[0]){
		case '+':  return dbms->union_of(name, r1, r2);
		case '-':  return dbms->difference(name, r1, r2);
		case '*':  return dbms->x_product(name, r1, r2);
		case 'J':  return dbms->natural_join(name, r1, r2); 
		default:
			throw Error(" **Parser Error: expr_query\n");
	}
} // end expr_query

/* condition(Relation *r, string input):
	A condition match is x op y {*}.  The condition can also start with an open parenthesis.
	A conjunction is || ({*}), and a comparison is && ({*}).  This function will evaluate each
	condition individually by calling relation::compare, and store each of the results in a 
	matrix of booleans.  The vector<bool> represents the indexes to the rows of a given relation.
	The || and && operations are saved in a seperate vector.  At the end, the algorithm works
	backwards through the matrix columns using the stored operations.
	Return:  A vector<bool> with 1's where the index matched everything, 0's otherwise.
*/
vector<bool> Parser::condition(Relation *r, string input){
	string attr, entry, opr, more;
	vector<bool> result(r->size(), false); // Initialize the result vector to the correct size
	vector< vector<bool> > matches;  // Matrix of comparisons
	vector<char> symbol;  // Will contain the operators || or &&
	
	sregex cond =
		(*_s >> (s1 = id)[ref(attr) = s1] >> *_s >> (s2 = op)[ref(opr) = s2] >> *_s >> (s3 = literals)[ref(entry) = s3] >> 
		*_s >> (s4 = *_)[ref(more) = s4]) | *_s >> ('(' >> *_s >> (s1 = id)[ref(attr) = s1] >> *_s >> 
		(s2 = op)[ref(opr) = s2] >> *_s >> (s3 = literals)[ref(entry) = s3] >> *_s >> (s4 = *_)[ref(more) = s4]);
	sregex conjunction =
		skip(_s)(as_xpr("||") >> (s1 = *_)[ref(more) = s1]);
	sregex comparison =
		skip(_s)(as_xpr("&&") >> (s1 = *_)[ref(more) = s1]);
	sregex parens =
		skip(_s)(*(as_xpr(')')));

	while(!input.empty()){
		if(regex_match(input, cond)){ // If there was a match, the variables will have data
			remove_quotes(entry);  // Take off quotation marks from literal inputs
			matches.push_back(result); // Result is currently just an empty vector
			matches.back() = r->compare(attr, entry, opr); // Populate the vector with matches
			input = more;
		}
		else if(regex_match(input, conjunction)){  // OR operation
			symbol.push_back('|');
			input = more;	
		}
		else if(regex_match(input, comparison)){  // AND operation
			symbol.push_back('&');
			input = more;
		}
		else if(regex_match(input, parens)){  // Nothing left, exit
			input = "";
		}
		else{
			throw Error("\n **Parser Error: Invalid condition");
		}
	}
	if((int) matches.size() == 1){ // Input was an expression with only one condition
		return matches[0];
	}
	// Work through the matrix from the back, performing appropriate comparisons
	// Ex: a &&(b ||(c)), does an OR on expr c and b, then an AND with the result and a.
	vector< vector<bool> >::reverse_iterator matches_rit;
	while(!symbol.empty()){
		matches_rit = matches.rbegin();
		vector<bool> last = *matches_rit;
		vector<bool> next_to_last = *(++matches_rit);
		switch(symbol.back()){
			case '|':
				for(int i = 0; i < r->size(); i++){
					if((last[i] == 1) || (next_to_last[i] == 1)){
						result[i] = true;
					}
				}
				break;
			case '&':
				for(int i = 0; i < r->size(); i++){
					if((last[i] == 1) && (next_to_last[i] == 1)){
						result[i] = true;
					}
					else{
						result[i] = false;
					}
				}
				break;
		}
		symbol.pop_back();  // This symbol has been dealt with
		matches.pop_back(); // These two expressions have been evaluated
		matches.pop_back();
		matches.push_back(result); // Put the result of the previous two in the matrix
	}
	return matches[0];
} // end condition

/* temp_name():
	Some relations are intermediate steps that are assigned no name.  In order
	for the database to keep track of these, a temporary name is assigned.  Those
	relations are then added to the database's vector of relations like any other.
	The '$' is not permitted in identifiers in the grammar, so it is used to precede
	the name.
	Return:  string with temporary name
*/
string Parser::temp_name(){
	string name = "$Temp";
	std::ostringstream oss;
	oss << name << n_temps;
	n_temps++;
	return oss.str();
}

/* show_vector(vector<string> &v, string title):
	Helper function to show the values in a vector
*/
void Parser::show_vector(vector<string> &v, string title){
	vector<string>::iterator it;
	cout << "\n" << title << " contains: ";
	for(it = v.begin(); it < v.end(); it++)
		cout << " " << *it;
}

/* remove_quotes(string &s):
	Removes quotes from a string.
*/
void Parser::remove_quotes(string &s){
	s.erase(remove(s.begin(), s.end(), '\"'), s.end());
}

/* remove_quotes(vector<string> &v):
	Removes quotes from all the strings in a vector
*/
void Parser::remove_quotes(vector<string> &v){
	vector<string>::iterator it;
	for(it = v.begin(); it < v.end(); it++){
		it->erase(remove(it->begin(), it->end(), '\"'), it->end());
	}
}

/* remove_semicolons(string &s):
	Removes semicolons from a string
*/
void Parser::remove_semicolons(string &s){
	s.erase(remove(s.begin(), s.end(), ';'), s.end());
}

/* set_database(Database *database):
	Establishes the parser's handle to the database object.
	This is only called by the database on its parser object.
*/
void Parser::set_database(Database *database){
	dbms = database;
}



