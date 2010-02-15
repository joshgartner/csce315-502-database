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

Parser::Parser(){
	// Set up Regex Matches
	id       = (alpha >> *(alpha | _d));
	literals = skip(_s)(('"' >> +alnum >> '"')| +_d);
	op       = skip(_s)(as_xpr("==") | "!=" | '<' | '>' | "<=" | ">=");
	type     = (icase("VARCHAR") >> '(' >> +_d >> ')') | icase("INTEGER");

	/* FIXME - All of this will move

	atomic_expr = relation_name | ('(' >> by_ref(expr) >> ')');

	selection = skip(_s)(icase("select") >> '(' >> condition >> ')' >> atomic_expr >> ';');

	projection = skip(_s)(icase("PROJECT") >> attr_list >> atomic_expr);

	renaming = skip(_s)(icase("RENAME") >> attr_list >> atomic_expr);

	union_of = skip(_s)(atomic_expr >> '+' >> atomic_expr);

	difference = skip(_s)(atomic_expr >> '-' >> atomic_expr);

	product = skip(_s)(atomic_expr >> '*' >> atomic_expr);

	natural_join = skip(_s)(atomic_expr >> icase("JOIN") >> atomic_expr);

	expr = (atomic_expr | selection | projection | renaming | union_of | difference | product | natural_join);

	query = skip(_s)(relation_name >> "<-" >> expr);
	
	program = (command | query);
	*/
}

/* FIXME - need to add union, natural-join, product, and difference
*/
Relation * Parser::match(string &input){
	int n = -1;
	sregex query = skip(_s)(icase("create")[ref(n) = CREATE] >> *_
	    | icase("update")[ref(n) = UPDATE] >> *_
		| icase("insert")[ref(n) = INSERT] >> *_
		| icase("delete")[ref(n) = DELETE] >> *_
		| icase("select")[ref(n) = SELECT] >> *_
		| icase("project")[ref(n) = PROJECT] >> *_
		| icase("rename")[ref(n) = RENAME] >> *_);

	regex_match(input, query);
	switch(n){
		case CREATE:
			return create_cmd(input);
			break;
		case UPDATE:
			return update_cmd(input);
			break;
		case INSERT:
			return insert_cmd(input);
			break;
		case DELETE:
			return delete_cmd(input);
			break;
		case SELECT:
		case PROJECT:
		case RENAME:
		default:
			throw Error(" **Parser Error: invalid input\n");
	}
}

// Make sure that the vector has been initialized to the size of relation before calling!
void Parser::condition(Relation *r, string input, vector<bool> &matches){
	string attr, entry, opr, more;
	int index; // A row number that matches the condition
	
	sregex cond = 
		skip(_s)((s1 = id)[ref(attr) = s1] >> (s2 = op)[ref(opr) = s2] >> (s3 = literals)[ref(entry) = s3] >> 
		(s4 = *_)[ref(more) = s4]) | skip(_s)('(' >> (s1 = id)[ref(attr) = s1] >> (s2 = op)[ref(opr) = s2] >> 
		(s3 = literals)[ref(entry) = s3] >> (s4 = *_)[ref(more) = s4]);
	sregex conjunction =
		skip(_s)("||" >> (s1 = *_)[ref(more) = s1]);
	sregex comparison =
		skip(_s)("&&" >> (s1 = *_)[ref(more) = s1]);
	sregex parens =
		skip(_s)(*(as_xpr(')')));

	if(regex_match(input, cond)){
		cout << "\nSuccessful condition match";
		cout << "\nAttr : " << attr;
		cout << "\nEntry: " << entry;
		cout << "\nOp   : " << opr;
		cout << "\nMore : " << more;
		index = r->compare(attr, entry, opr);
		if(index > 0){
			matches[index] = true; // Add to rows matching the condition
		}
		if(more != ""){
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

Relation * Parser::create_cmd(string &cmd){
	string name;
	vector<string> v_attr, v_types, v_keys;

	// Establish criteria for a match, while simultaneously populating vectors with data
	sregex keys = 
		skip(_s)('(' >> (s1 = id)[vpush(ref(v_keys), s1)] >> *(',' >> (s2 = id)[vpush(ref(v_keys), s2)]) >> ')');

	sregex typed_attr_list = 
		skip(_s)('(' >> (s1 = id)[vpush(ref(v_attr), s1)] >> (s2 = type)[vpush(ref(v_types), s2)] >> 
		*(',' >> (s3 = id)[vpush(ref(v_attr), s3)] >> (s4 = type)[vpush(ref(v_types), s4)]) >> ')');

	sregex cmd_create = 
		skip(_s)(icase("CREATE TABLE") >> (s1 = id)[ref(name) = s1] >> typed_attr_list >>
		icase("PRIMARY KEY") >> keys >> ';');

	if(regex_match(cmd, cmd_create)){
		Relation *r = new Relation(name, v_attr, v_types, v_keys);
		cout << "Relation Created: " << name;
		show_vector(v_attr, "Attributes");
		show_vector(v_types, "Types");
		show_vector(v_keys, "Primary Keys");
		return r;
	}
	else{
		throw Error(" **Parser Error: invalid input\n");
	}
}

Relation * Parser::insert_cmd(string &cmd){
	string name = "NULL";
	string expr = "NULL";
	vector<string> v_tuple;

	sregex cmd_insert = 
		skip(_s)(icase("INSERT INTO") >> (s1 = id)[ref(name) = s1] >> icase("VALUES FROM") >>
		'(' >> (s2 = literals)[vpush(ref(v_tuple), s2)] >> *(',' >> (s3 = literals)[vpush(ref(v_tuple), s3)]) >> 
		')' >> ';') | skip(_s)(icase("INSERT INTO") >> (s4 = id)[ref(name) = s4] >> icase("VALUES FROM RELATION") >>
		(s5 = *_)[ref(expr) = s5]);

	// FIXME - Handle "values from relation" case.
	if(regex_match(cmd, cmd_insert)){
		Relation *r = dbms->get_relation(name);
		cout << "\nRelation Fetched:";
		cout << "\nRelation Name : " << name;
		cout << "\nFollow Expr is: " << expr;
		remove_quotes(v_tuple);
		show_vector(v_tuple, "Tuples");
		return r;
	}
	else{
		throw Error(" **Parser Error: invalid input\n");
	}
}


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
		int index = 0;
		for(unsigned int i = 0; i < matches.size(); i++){
			if(matches[i] == true){
				index = i; // Only one row can match
				cout << "Assigned index in update: " << index;
				break;
			}
		}
		r->update_attrs(v_attr, v_literals, index);
		cout << "\nRelation Updated: " << name;
		show_vector(v_attr, "Attributes");
		show_vector(v_literals, "Literals");
		cout << "\nCondition: " << cond << "\n";
		return r;
	}
	else{
		throw Error(" **Parser Error: invalid input\n");
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
		int index = 0;
		for(unsigned int i = 0; i < matches.size(); i++){
			if(matches[i] == true){
				index = i; // Only one row can match
				cout << "Assigned index in update: " << index;
				break;
			}
		}
		cout << "\nDeleting from: " << name;
		cout << "\nCondition    : " << cond;
		r->remove_row(index);
		return r;
	}
	else{
		throw Error(" **Parser Error: invalid input\n");
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


