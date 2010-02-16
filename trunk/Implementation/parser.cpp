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
	id         = (alpha >> *(alpha | _d));
	literals   = skip(_s)(('"' >> +alnum >> '"')| +_d);
	op         = skip(_s)(as_xpr("==") | "!=" | '<' | '>' | "<=" | ">=");
	type       = (icase("VARCHAR") >> '(' >> +_d >> ')') | icase("INTEGER");
	
	query_op   = (as_xpr('+') | as_xpr('-') | as_xpr('*') | icase("JOIN"));

	atomic_expr = skip(_s)('(' >> icase("select") >> *_
		| '(' >> icase("project") >> *_
		| '(' >> icase("rename") >> *_)
		| id;
}

/* FIXME
*/
Relation * Parser::match(string &input){
	int n = -1;
	string name;
	smatch what; // Contains what matched
	sregex query = 
		skip(_s)(icase("create")[ref(n) = CREATE] >> *_
	    | icase("update")[ref(n) = UPDATE] >> *_
		| icase("insert")[ref(n) = INSERT] >> *_
		| icase("delete")[ref(n) = DELETE] >> *_
		| id >> "<-" >> icase("select")[ref(n) = SELECT] >> *_
		| id >> "<-" >> icase("project")[ref(n) = PROJECT] >> *_
		| id >> "<-" >> icase("rename")[ref(n) = RENAME] >> *_
		| (s1 = id)[ref(name) = s1] >> "<-" >> *_
		| (s1 = id)[ref(name) = s1]);

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
	else if(name != ""){
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
		skip(_s)(as_xpr("||") >> (s1 = *_)[ref(more) = s1]);
	sregex comparison =
		skip(_s)(as_xpr("&&") >> (s1 = *_)[ref(more) = s1]);
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
		cout << "\nRelation <" << name << "> created."; // DEBUG only
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


Relation * Parser::select_query(string &cmd){
	string name, cond, more;
	sregex conditions = 
		skip(_s)((s1 = before(atomic_expr))[ref(cond) = s1]);

	sregex selection = 
		skip(_s)((s1 = id)[ref(name) = name] >> "<-" >> icase("select") >> conditions >> 
		(s2 = atomic_expr)[ref(more) = s2]);

	// FIXME - Adapt to call dbms->select
	if(regex_match(cmd, selection)){
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
				r->add_row(from_relation->get_row(i));
			}
		}
		return r;
	}
	else
		throw Error(" **Parser Error: invalid input\n");
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
		throw Error(" **Parser Error: invalid input\n");
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
		throw Error(" **Parser Error: invalid input\n");
}


Relation * Parser::expr_query(string &name, Relation *r1, Relation *r2, string operation){
	switch(operation[0]){
		case '+':  return dbms->union_of(name, r1, r2);
		case '-':  return dbms->difference(name, r1, r2);
		case '*':  return dbms->x_product(name, r1, r2);
		case 'J':
		case 'j':  return dbms->natural_join(name, r1, r2);
		default:
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


