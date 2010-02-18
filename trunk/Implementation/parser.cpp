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
	id         = (alpha >> *(alpha | _d));
	literals   = skip(_s)(('"' >> +alnum >> '"')| +_d);
	op         = skip(_s)(as_xpr("==") | "!=" | '<' | '>' | "<=" | ">=");
	type       = (icase("VARCHAR") >> '(' >> +_d >> ')') | icase("INTEGER");
	
	query_op   = (as_xpr('+') | (~(boost::xpressive::set['<']) >> *_s >> as_xpr('-')) 
		| as_xpr('*') | icase("JOIN"));

	atomic_expr = ('(' >> *_s >> icase("select") >> *_s >> *_
		| '(' >> *_s >> icase("project") >> *_s >> *_
		| '(' >> *_s >> icase("rename") >> *_s >> *_
		| id >> *_s >> ';');
}

/* match(string &input):
	This is the entry point for the Parser module.  It establishes some match
	criteria, searches for operations that need to be broken up like union, difference,
	product, and natural_join, and splits around them if necessary.
	Return: A pointer to the relation created.
*/
Relation * Parser::match(string &input){
	int n = -1;
	string name;
	smatch what; // Contains what matched
	sregex query = 
		( *_s >> icase("create")[ref(n) = CREATE] >> *_
	    | *_s >> icase("update")[ref(n) = UPDATE] >> *_
		| *_s >> icase("insert")[ref(n) = INSERT] >> *_
		| *_s >> icase("delete")[ref(n) = DELETE] >> *_
		| *_s >> id >> *_s >> "<-" >> *_s >> icase("select")[ref(n) = SELECT] >> *_
		| *_s >> id >> *_s >> "<-" >> *_s >> icase("project")[ref(n) = PROJECT] >> *_
		| *_s >> id >> *_s >> "<-" >> *_s >> icase("rename")[ref(n) = RENAME] >> *_
		| *_s >> (s1 = id)[ref(name) = s1] >> *_s >> "<-" >> *_s >> *_
		| *_s >> (s1 = id)[ref(name) = s1] >> *_s);

	regex_match(input, query);

	if(regex_search(input, what, query_op)){
		size_t found = input.find(what[0]);
		string left, right;
		left = input.substr(0, found-1);
		if(what[0] == "JOIN")
			right = input.substr(found+4, input.size()-found-1);
		else
			right = input.substr(found+1, input.size()-found-1);
		Relation *r1 = match(left);
		Relation *r2 = match(right);
		return expr_query(name, r1, r2, (string) what[0]);
	}
	else if(!name.empty()){
		return dbms->get_relation(name);
	}

	switch(n){
		case CREATE :  return create_cmd(input);
		case UPDATE :  return update_cmd(input);
		case INSERT :  return insert_cmd(input);
		case DELETE :  return delete_cmd(input);
		case SELECT :  return select_query(input);
		case PROJECT:  return project_query(input);
		case RENAME :  return rename_query(input);
		default:
			throw Error(" **Parser Error: match failed\n");
	}
}

/* condition(Relation *r, string input, vector<bool> &matches):
	A condition match is x op y {*}.  Or the condition can start with an open parenthesis.
	A conjunction is || ({*}), and a comparison is && ({*}).
*/
void Parser::condition(Relation *r, string input, vector<bool> &matches){
	string attr, entry, opr, more;
	
	sregex cond =
		(*_s >> (s1 = id)[ref(attr) = s1] >> *_s >> (s2 = op)[ref(opr) = s2] >> *_s >> (s3 = literals)[ref(entry) = s3] >> 
		*_s >> (s4 = *_)[ref(more) = s4]) | *_s >> ('(' >> *_s >> (s1 = id)[ref(attr) = s1] >> *_s >> 
		(s2 = op)[ref(opr) = s2] >> *_s >> (s3 = literals)[ref(entry) = s3] >> *_s >> (s4 = *_)[ref(more) = s4]);
	sregex conjunction =
		skip(_s)(as_xpr("||") >> (s1 = *_)[ref(more) = s1]);
	sregex comparison =
		skip(_s)(as_xpr("&&") >> (s1 = *_)[ref(more) = s1]);
	sregex parens =
		skip(_s)(*(as_xpr(')')) | *(as_xpr(')')) >> ';');

	if(regex_match(input, cond)){
		cout << "\nSuccessful condition match";
		cout << "\nAttr : " << attr;
		cout << "\nEntry: " << entry;
		cout << "\nOp   : " << opr;
		cout << "\nMore : " << more;
		r->compare(attr, entry, opr, matches);
		if(!more.empty()){
			cout << "\nSending next condition:";
			condition(r, more, matches);
		}
	}
	else if(regex_match(input, conjunction)){
		condition(r, more, matches); // Continue to add to matches on an OR
	}
	else if(regex_match(input, comparison)){
		vector<bool> temp = matches;
		condition(r, more, matches);
		vector<bool>::iterator it1;
		vector<bool>::iterator it2;
		for(it1 = temp.begin(); it1 < temp.end(); it1++){
			for(it2 = matches.begin(); it2 < matches.end(); it2++){
				if(*it1 != *it2){
					*it2 = false; // Clear out the non-matching conditions
				}
			}
		}
	}
	else if(!regex_match(input, parens)){
		// There was something more than closing parenthesis left:
		string msg = " **invalid condition at: " + input;
		throw Error(msg);
	}
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
		icase("PRIMARY KEY") >> *_s >> keys >> *_s >> ';');

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
		throw Error(" **Parser Error: create command\n");
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
		(s3 = literals)[vpush(ref(v_tuple), s3)]) >> *_s >> ')' >> *_s >> ';') | (icase("INSERT INTO") >> *_s >> 
		(s4 = id)[ref(name) = s4] >> *_s >> icase("VALUES FROM RELATION") >> *_s >> (s5 = *_)[ref(expr) = s5]);

	if(regex_match(cmd, cmd_insert)){
		Relation *r = dbms->get_relation(name);
		cout << "\nRelation Name:" << name;
		if(expr.empty()){
			remove_quotes(v_tuple);
			show_vector(v_tuple, "Tuples");
			r = dbms->insert(r, v_tuple);
		}
		else{
			cout << "\nFollow Expr is:" << expr;
			Relation *from_relation = match(expr);
			r = dbms->insert(r, from_relation);
		}
		return r;
	}
	else{
		throw Error(" **Parser Error: insert command\n");
	}
}

/* update_cmd(string &cmd):
	
*/
Relation * Parser::update_cmd(string &cmd){
	string name, cond;
	vector<string> v_attr, v_literals;

	sregex cmd_update = 
		skip(_s)(icase("UPDATE") >> (s1 = id)[ref(name) = s1] >> icase("SET") >> (s2 = id)[vpush(ref(v_attr), s2)] >> 
		"=" >> (s3 = literals)[vpush(ref(v_literals), s3)] >> *(',' >> (s4 = id)[vpush(ref(v_attr), s4)] >> "=" >> 
		(s5 = literals)[vpush(ref(v_literals), s5)]) >> icase("WHERE") >> (s6 = *_)[ref(cond) = s6]);

	if(regex_match(cmd, cmd_update)){
		Relation *r = dbms->get_relation(name);
		int size = r->size();
		vector<bool> matches(size, false);
		condition(r, cond, matches);   // Matches now has index to row we want.
		remove_quotes(v_literals);
		for(unsigned int i = 0; i < matches.size(); i++){
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
		throw Error(" **Parser Error: update command\n");
	}
}



Relation * Parser::delete_cmd(string &cmd){
	string name, cond;

	sregex cmd_delete = 
		skip(_s)(icase("DELETE FROM") >> (s1 = id)[ref(name) = s1] >> icase("WHERE") >> 
		(s2 = *_)[ref(cond) = s2]);

	if(regex_match(cmd, cmd_delete)){
		Relation *r = dbms->get_relation(name);
		int size = r->size();
		vector<bool> matches(size, false);
		condition(r, cond, matches);   // Matches now has index to row we want.
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
		throw Error(" **Parser Error: delete command\n");
	}
}


Relation * Parser::select_query(string &cmd){
	string name, cond, more;
	smatch what;
	sregex conditions = (s1 = before(atomic_expr))[ref(cond) = s1];

	sregex selection = 
		(*_s >> (s1 = id)[ref(name) = name] >> *_s >> "<-" >> *_s >> icase("select") >> *_s >> conditions >> 
		 *_s >> (s2 = atomic_expr)[ref(more) = s2]);

	if(regex_match(cmd, what, selection)){
		cout << "\nRelation name  : " << name;
		cout << "\nCondtions were : " << cond;
		cout << "\nFrom expression: " << more;
		Relation *r = new Relation;
		Relation *from_relation = match(more);
		int size = from_relation->size();
		vector<bool> matches(size, false);
		matches[0] = true;                         // The attr row is always true
		condition(from_relation, cond, matches);   // Matches now has indexes to rows we want.
		for(unsigned int i = 0; i < matches.size(); i++){
			if(matches[i] == true){
				r->add_tuple(from_relation->get_row(i));
			}
		}
		return r;
	}
	else{
		cout << what[0];
		throw Error(" **Parser Error: select_query\n");
	}
}

Relation * Parser::project_query(string &cmd){
	string name, more;
	vector<string> v_attrs;
	sregex attr_list = 
		skip(_s)('(' >> (s1 = id)[vpush(ref(v_attrs), s1)] >> *(',' >> (s2 = id)[vpush(ref(v_attrs), s2)]) >> ')');

	sregex projection = 
		skip(_s)((s1 = id)[ref(name) = name] >> "<-" >> icase("project") >> attr_list >> 
		(s2 = atomic_expr)[ref(more) = s2]);

	if(regex_match(cmd, projection)){
		cout << "\nRelation name  : " << name;
		cout << "\nFrom expression: " << more;
		show_vector(v_attrs, "Attributes");
		Relation *r = new Relation;
		Relation *from_relation = match(more);
		r = dbms->project(name, from_relation, v_attrs);
		return r;
	}
	else
		throw Error(" **Parser Error: project_query\n");
}

Relation * Parser::rename_query(string &cmd){
	string name, more;
	vector<string> v_attrs;
	sregex attr_list = 
		skip(_s)('(' >> (s1 = id)[vpush(ref(v_attrs), s1)] >> *(',' >> (s2 = id)[vpush(ref(v_attrs), s2)]) >> ')');

	sregex renaming = 
		skip(_s)((s1 = id)[ref(name) = name] >> "<-" >> icase("rename") >> attr_list >> 
		(s2 = atomic_expr)[ref(more) = s2]);

	if(regex_match(cmd, renaming)){
		cout << "\nRelation name  : " << name;
		cout << "\nFrom expression: " << more;
		show_vector(v_attrs, "Attributes");
		Relation *r = new Relation;
		Relation *from_relation = match(more);
		r = dbms->rename(name, from_relation, v_attrs);
		return r;
	}
	else
		throw Error(" **Parser Error: rename_query\n");
}


Relation * Parser::expr_query(string &name, Relation *r1, Relation *r2, string operation){
	switch(operation[0]){
		case '+':  return dbms->union_of(name, r1, r2);
		case '-':  return dbms->difference(name, r1, r2);
		case '*':  return dbms->x_product(name, r1, r2);
		case 'J':
		case 'j':  return dbms->natural_join(name, r1, r2);
		default:
			throw Error(" **Parser Error: expr_query\n");
	}
}

void Parser::show_vector(vector<string> &v, string title){
	vector<string>::iterator it;
	cout << "\n" << title << " contains: ";
	for(it = v.begin(); it < v.end(); it++)
		cout << " " << *it;
}

void Parser::remove_quotes(vector<string> &v){
	vector<string>::iterator it;
	for(it = v.begin(); it < v.end(); it++){
		it->erase(remove(it->begin(), it->end(), '\"'), it->end());
	}
}

void Parser::set_database(Database *database){
	dbms = database;
}

