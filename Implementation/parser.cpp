#include "parser.hpp"

class Error{
public:
	string message;
	Error(string msg){
		message = msg;
	}
};

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
	op       = (as_xpr("==") | "!=" | '<' | '>' | "<=" | ">=");
	type     = (icase("VARCHAR") >> '(' >> +_d >> ')') | icase("INTEGER");

	/* FIXME - All of this will move
	comparison = skip(_s)(operand >> op >> operand);

	condition = comparison >> (*(as_xpr("&&") >> comparison) | *("||" >> comparison)) |
		       (*(as_xpr("&&") >> '(' >> comparison >> (*("&&" >> comparison) | *("||" >> comparison)) >> ')')) |
			   (*(as_xpr("||") >> '(' >> comparison >> (*("&&" >> comparison) | *("||" >> comparison)) >> ')'));

	relation_name = (s1 = identifier)[ref(r_name) = s1];

	attr_name     = (s1 = identifier)[vpush(ref(v_attr_names), s1)];

	attr_list     = skip(_s)('(' >> attr_name >> *(',' >> attr_name) >> ')');

	operand = attr_name | literals;

	cmd_update = skip(_s)(icase("UPDATE") >> relation_name >> icase("SET") >> 
		         attr_name >> '=' >> literals >> *(',' >> attr_name >> '=' >> literals) >> 
		         icase("WHERE") >> condition >> ';');

	
				 
	cmd_delete = skip(_s)(icase("DELETE FROM") >> relation_name >> icase("WHERE") >> condition >> ';');

	command    = +(cmd_create | cmd_update | cmd_insert | cmd_delete);

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

bool Parser::match(string &input){
	int n = -1;
	sregex query = skip(_s)(icase("create")[ref(n) = CREATE] >> *_
	    | icase("update")[ref(n) = UPDATE] >> *_
		| icase("insert")[ref(n) = INSERT] >> *_
		| icase("delete")[ref(n) = DELETE] >> *_
		| icase("select")[ref(n) = SELECT] >> *_
		| icase("project")[ref(n) = PROJECT] >> *_
		| icase("rename")[ref(n) = RENAME] >> *_);

	regex_match(input, query);
	try{
		switch(n){
			case CREATE:
				create_cmd(input);
				break;
			case UPDATE:
			case INSERT:
				insert_cmd(input);
				break;
			case DELETE:
			case SELECT:
			case PROJECT:
			case RENAME:
			default:
				throw Error(" **Parser Error: invalid input\n");
		}
	}catch(Error e){
		cout << e.message;
		return false;
	}	
	return true;
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

/*
Relation * Parser::update_cmd(string &cmd){
}



Relation * Parser::delete_cmd(string &cmd){
}
*/


void Parser::show_vector(vector<string> &v, string title){
	vector<string>::iterator it;
	cout << "\n" << title << " contains: ";
	for(it = v.begin() ; it < v.end(); it++)
		cout << " " << *it;
}

void Parser::remove_quotes(vector<string> &v){
	vector<string>::iterator it;
	for(it = v.begin() ; it < v.end(); it++){
		it->erase(remove(it->begin(), it->end(), '\"'), it->end());
	}
}

void Parser::set_database(Database *database){
	dbms = database;
}


