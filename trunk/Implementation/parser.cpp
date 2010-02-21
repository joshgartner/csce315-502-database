#include "parser.hpp"

// Semantic Action, pushes argument into a vector
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
	parsing operations.
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

	n_temps = 0;
}

/* match(string &input):
	This is the entry point for the Parser module.  It establishes some match
	criteria, searches for operations that need to be broken up like union, difference,
	product, and natural_join, and splits around them if necessary.
	Return: A pointer to the relation created.
*/
Relation * Parser::match(string &input){
	cout << "\nMatching:" << input;
	int n = -1;
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
			if(name.empty()){
				name = temp_name();
			}
			cout << "\nExpr1:" << expr1;
			cout << "\nExpr2:" << expr2;
			r1 = match(expr1); 
			r2 = match(expr2);
			r = expr_query(name, r1, r2, expr_op);
			if(name[0] == '$'){
				dbms->add_relation(r);
			}
			return r;
		default:
			if(!name.empty()){
				return dbms->get_relation(name);
			}
			else
				throw Error("\n **Parser Error: invalid input\n");
	}
}

/* condition(Relation *r, string input, vector<bool> &matches):
	A condition match is x op y {*}.  Or the condition can start with an open parenthesis.
	A conjunction is || ({*}), and a comparison is && ({*}).  This function will evaluate each
	condition by calling relation::compare, and store each of the results in a matrix of booleans.
	The || and && operations are saved in a seperate vector.  At the end, the algorithm works
	backwards through the matrix columns using the conditions.
	Return:  A vector<bool> with 1's where the index matched everything.
*/
vector<bool> Parser::condition(Relation *r, string input){
	string attr, entry, opr, more;
	vector<bool> result(r->size(), false);
	vector< vector<bool> > matches;
	vector<char> symbol;
	
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
		if(regex_match(input, cond)){
			remove_quotes(entry);  
			// DEBUG cout << "\nSuccessful condition match";
			// DEBUG cout << "\nAttr :" << attr;
			// DEBUG cout << "\nEntry:" << entry;
			// DEBUG cout << "\nOp   :" << opr;
			// DEBUG cout << "\nMore :" << more;
			matches.push_back(result); // result will just be an empty placeholder vector
			matches.back() = r->compare(attr, entry, opr);
			input = more;
		}
		else if(regex_match(input, conjunction)){
			symbol.push_back('|');
			input = more;	
		}
		else if(regex_match(input, comparison)){
			symbol.push_back('&');
			input = more;
		}
		else if(regex_match(input, parens)){
			// DEBUG cout << "\nEnd of condtion reached";
			input = "";
		}
		else{
			throw Error("\n **Parser Error: Invalid condition");
		}
	}
	if((int) matches.size() == 1){
		return matches[0];
	}
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
					// DEBUG cout << "\nLast: " << last[i] << " Next to last: " << next_to_last[i];
					if((last[i] == 1) && (next_to_last[i] == 1)){
						
						result[i] = true;
					}
					else{
						result[i] = false;
					}
				}
				break;
		}
		symbol.pop_back();
		matches.pop_back();
		matches.pop_back();
		matches.push_back(result);
	}
	return matches[0];
}

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
		cout << "Trying to make relation:" << name;
		show_vector(v_attr, "Attributes");
		show_vector(v_types,"Types     ");
		show_vector(v_keys, "Keys      ");
		Relation *r = new Relation(name, v_attr, v_types, v_keys);
		cout << "\nRelation <" << name << "> created."; // DEBUG only
		return r;
	}
	else{
		throw Error("\n **Parser Error: create command\n");
	}
}

/* insert_cmd(string &cmd):
	Parses and tokenizes insert commands.  On a match, pulls the the required relation from the database and
	calls insert on it.  In the FROM RELATION case, it evaluates the expr, and calls the overloaded insert.
	Returns: The relation created
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
		cout << "\nRelation Name:" << name;
		if(expr.empty()){
			remove_quotes(v_tuple);
			show_vector(v_tuple, "Tuples");
			r->add_tuple(v_tuple);
		}
		else{
			cout << "\nFollow Expr is:" << expr;
			Relation *from_relation = match(expr);
			r = dbms->insert(r, from_relation);
		}
		return r;
	}
	else{
		throw Error("\n **Parser Error: insert command\n");
	}
}

/* update_cmd(string &cmd):
	
*/
Relation * Parser::update_cmd(string &cmd){
	string name, cond;
	vector<string> v_attr, v_literals;

	sregex cmd_update = 
		(icase("UPDATE") >> *_s >> (s1 = id)[ref(name) = s1] >> *_s >> icase("SET") >> *_s >>
		(s2 = id)[vpush(ref(v_attr), s2)] >> *_s >> "=" >> *_s >> (s3 = literals)[vpush(ref(v_literals), s3)] >>
		*_s >> *(',' >> *_s >> (s4 = id)[vpush(ref(v_attr), s4)] >> *_s >> "=" >> *_s >> 
		(s5 = literals)[vpush(ref(v_literals), s5)]) >> *_s >> icase("WHERE") >> *_s >> (s6 = *_)[ref(cond) = s6]);

	if(regex_match(cmd, cmd_update)){
		Relation *r = dbms->get_relation(name);
		int size = r->size();
		vector<bool> matches;
		matches = condition(r, cond);   // Matches now has index to row we want.
		remove_quotes(v_literals);
		for(int i = 0; i < (int) matches.size(); i++){
			if(matches[i] == true){
				r->update_attrs(v_attr, v_literals, i);
				cout << "Assigned index in update: " << i;
			}
		}
		cout << "\nRelation Updated: " << name;
		show_vector(v_attr, "Attributes");
		show_vector(v_literals, "Literals");
		cout << "\nCondition: " << cond << "\n";
		return r;
	}
	else{
		throw Error("\n **Parser Error: update command\n");
	}
}



Relation * Parser::delete_cmd(string &cmd){
	string name, cond;

	sregex cmd_delete = 
		(icase("DELETE FROM") >> *_s >> (s1 = id)[ref(name) = s1] >> *_s >> icase("WHERE") >> *_s >> 
		(s2 = *_)[ref(cond) = s2]);

	if(regex_match(cmd, cmd_delete)){
		Relation *r = dbms->get_relation(name);
		int size = r->size();
		vector<bool> matches;
		matches = condition(r, cond);   // Matches now has index to row we want.
		cout << "\nDeleting from: " << name;
		cout << "\nCondition    : " << cond;
		for(unsigned int i = 0; i < matches.size(); i++){
			if(matches[i] == true){
				r->remove_row(i);
				cout << "\nDeleted row: " << i;
			}
		}
		return r;
	}
	else{
		throw Error("\n **Parser Error: delete command\n");
	}
}


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
		cout << "\nRelation name  :" << name;
		cout << "\nCondtions were :" << cond;
		cout << "\nFrom expression:" << more;
		Relation *r = new Relation;
		Relation *from_relation = match(more);
		from_relation->copy_attrs(r);
		r->name = name;
		vector<bool> matches;
		matches = condition(from_relation, cond);   // Matches now has indexes to rows we want.
		for(int i = 0; i < (int) matches.size(); i++){
			if(matches[i] == true){
				r->add_tuple(from_relation->get_tuple(i));
			}
		}
		if(name[0] == '$'){
			dbms->add_relation(r);
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: select_query\n");
}

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
		if(name.empty()){
			name = temp_name();
		}
		cout << "\nRelation name  : " << name;
		cout << "\nFrom expression: " << more;
		show_vector(v_attrs, "Attributes");
		Relation *r = new Relation;
		Relation *from_relation = match(more);
		r = dbms->project(name, from_relation, v_attrs);
		if(name[0] == '$'){
			dbms->add_relation(r);
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: project_query\n");
}

Relation * Parser::rename_query(string &cmd){
	string name, more;
	vector<string> v_attrs;
	sregex attr_list = 
		('(' >> *_s >> (s1 = id)[vpush(ref(v_attrs), s1)] >> *_s >> *(',' >> *_s >> (s2 = id)[vpush(ref(v_attrs), s2)]) >>
		*_s >> ')');

	sregex renaming = 
		(*_s >> (s1 = id)[ref(name) = name] >> *_s >> "<-" >> *_s >> icase("rename") >> *_s >>
		attr_list >> *_s >> (s2 = atomic_expr)[ref(more) = s2])
		| (*_s >> *(as_xpr('(')) >> *_s >> icase("rename") >> *_s >> attr_list >> *_s >> 
		(s2 = atomic_expr)[ref(more) = s2] >> *(as_xpr(')')));

	if(regex_match(cmd, renaming)){
		if(name.empty()){
			name = temp_name();
		}
		cout << "\nRelation name  : " << name;
		cout << "\nFrom expression: " << more;
		show_vector(v_attrs, "Attributes");
		Relation *r = new Relation;
		Relation *from_relation = match(more);
		r = dbms->rename(name, from_relation, v_attrs);
		if(name[0] == '$'){
			dbms->add_relation(r);
		}
		return r;
	}
	else
		throw Error("\n **Parser Error: rename_query\n");
}


Relation * Parser::expr_query(string &name, Relation *r1, Relation *r2, string operation){
	switch(operation[0]){
		case '+':  return dbms->union_of(name, r1, r2);
		case '-':  return dbms->difference(name, r1, r2);
		case '*':  return dbms->x_product(name, r1, r2);
		case 'J':  return dbms->natural_join(name, r1, r2); 
		default:
			throw Error(" **Parser Error: expr_query\n");
	}
}

string Parser::temp_name(){
	string name = "$Temp";
	std::ostringstream oss;
	oss << name << n_temps;
	n_temps++;
	return oss.str();
}

void Parser::show_vector(vector<string> &v, string title){
	vector<string>::iterator it;
	cout << "\n" << title << " contains: ";
	for(it = v.begin(); it < v.end(); it++)
		cout << " " << *it;
}

void Parser::remove_quotes(string &s){
	s.erase(remove(s.begin(), s.end(), '\"'), s.end());
}

void Parser::remove_quotes(vector<string> &v){
	vector<string>::iterator it;
	for(it = v.begin(); it < v.end(); it++){
		it->erase(remove(it->begin(), it->end(), '\"'), it->end());
	}
}

void Parser::remove_semicolons(string &s){
	s.erase(remove(s.begin(), s.end(), ';'), s.end());
}

void Parser::set_database(Database *database){
	dbms = database;
}



