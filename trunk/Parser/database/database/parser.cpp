#include "parser.hpp"

// Semantic Action, pushes argument into a vector
struct pusher{
    typedef void result_type; // Result type, needed for xpressive

    template<typename Vector, typename Value>
    void operator()(Vector &vec, Value const &val) const{
        vec.push_back(val);
    }
};

function<pusher>::type const push_arg = {{}};

Parser::Parser(){
	// Set up Regex Matches
	literals   = (as_xpr('"') >> *_s >> +alnum >> *_s >> '"' | +_d);
	condition  = +_w; // FIXME

	type       = (s1 = (icase("VARCHAR") >> '(' >> +_d >> ')'))[push_arg(ref(data2), s1)] | 
		         (s1 = icase("INTEGER"))[push_arg(ref(data2), s1)];

	typed_attr_list  = '(' >> (s1 = +_w) [push_arg(ref(data1), s1)] >> +_s >> type >> 
		               *(*_s >> ',' >> *_s >> (s2 = +_w) [push_arg(ref(data1), s2)] >> +_s >> type) >> ')';

	cmd_create = (icase("CREATE TABLE") >> +_s >> (s1 = +_w)[ref(r_name) = s1] 
	             >> +_s >> typed_attr_list >> +_s >>
		         icase("PRIMARY KEY")  >> +_s >> '(' >> *_s >> +_w >> *(*_s >> ',' >> *_s >> +_w) >>
				 ')' >> *_s >> ';');

	cmd_update = icase("UPDATE") >> +_s >> +_w >> +_s >> icase("SET") >> +_s >>
		         +_w >> *_s >> '=' >> *_s >> +_w >>
				 *(*_s >> ',' >> *_s >> +_w >> *_s >> '=' >> *_s >> +_w) >> +_s >> 
		         icase("WHERE") >> +_s >> condition >> *_s >> ';';

	cmd_insert = icase("INSERT INTO") >> +_s >> +_w >> +_s >> icase("VALUES FROM") >> +_s >>
		         '(' >> *_s >> literals >> *(*_s >> ',' >> *_s >> literals) >> ')' >> *_s >> ';';
				 
	cmd_delete = icase("DELETE FROM") >> +_s >> +_w >> +_s >> icase("WHERE") >>
		         +_s >> condition >> *_s >> ';';

	command    = +(cmd_create | cmd_update | cmd_insert | cmd_delete);

	identifier = +alpha >> *(alpha | _d);

	relation_name = identifier;
	attr_name     = identifier;
	attr_list     = (as_xpr('(') >> attr_name >> *(*_s >> ',' >> *_s >> attr_name) >> *_s >> ')');

	atomic_expr = (relation_name | (by_ref(expr)));

	selection = (icase("select") >> +_s >> '(' >> *_s >> condition >> *_s >> ')' >> +_s >> atomic_expr);

	op = (as_xpr("==") | "!=" | '<' | '>' | "<=" | ">=");

	operand = attr_name | literals;

	comparison = ((operand >> *_s >> op >> *_s >> operand) | (*_s >> '(' >> *_s >> by_ref(condition) >>
		         *_s >> ')'));

	conjunction = comparison >> *(*_s >> "&&" >> *_s >> comparison);

	condition = conjunction >> *(*_s >> "||" >> *_s >> conjunction);

	projection = (icase("PROJECT") >> +_s >> attr_list >> +_s >> atomic_expr);

	renaming = (icase("RENAME") >> +_s >> attr_list >> +_s >> atomic_expr);

	union_of = atomic_expr >> *_s >> '+' >> *_s >> atomic_expr;

	difference = atomic_expr >> *_s >> '-' >> *_s >> atomic_expr;

	product = atomic_expr >> *_s >> '*' >> *_s >> atomic_expr;

	natural_join = atomic_expr >> *_s >> icase("JOIN") >> *_s >> atomic_expr;

	expr = (atomic_expr | selection | projection | renaming | union_of | difference | product | natural_join);

	query = (relation_name >> *_s >> "<-" >> *_s >> expr);

	program = (command | query);
}

Parser::~Parser(){
}

bool Parser::match(std::string& input){
	return regex_match(input, program);
}

