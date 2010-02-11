#ifndef _parser_hpp_      // include file only once
#define _parser_hpp_

#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace boost::xpressive;

class Parser{
public:
	Parser();
	~Parser();

	bool match(std::string& input);
	
private:
	std::string r_name;              // Name of the relation
	std::vector<std::string> data1;  // Ex: attribute names
	std::vector<std::string> data2;  // Ex: primary keys

	// String Regular Expressions (sregex) for commands
	sregex command, cmd_create, cmd_update, cmd_insert, cmd_delete;
	sregex attr_list, type, literals;
	sregex condition;

	// Create the Grammar for for queries
	sregex query, relation_name, expr, identifier, atomic_expr;
	sregex selection, condition, conjunction, comparison;
	sregex op, operand, projection, renaming, union_of, difference;
	sregex product, natural_join, attr_name;
	sregex program;
};

#endif