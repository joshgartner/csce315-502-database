#ifndef _parser_hpp_      // include file only once
#define _parser_hpp_

#include "relation.hpp"
#include "database.hpp"
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Forward Declarations
class Database;

using namespace boost::xpressive;
using std::vector;
using std::string;
using std::cout;

enum { CREATE, UPDATE, INSERT, DELETE, SELECT, PROJECT,
       RENAME, UNION, DIFFERENCE, PRODUCT, NATURAL_JOIN };

class Parser{
public:
	Parser();

	smatch what;  // Holds what matched
	Database *dbms; // Handle to the database
	sregex id, literals, op, type; // Regexes for terminals

	// String Regular Expressions (sregex) for commands
	sregex command, cmd_update, cmd_delete;

	// Create the Grammar for for queries
	sregex query, relation_name, expr, identifier, atomic_expr;
	sregex selection, condition, conjunction, comparison;
	sregex operand, projection, renaming, union_of, difference;
	sregex product, natural_join, attr_name, attr_list;
	sregex program;

	bool match(string &input);  // FIXME - should return a relation probably
	Relation * create_cmd(string &cmd);
	//Relation * update_cmd(string &cmd);
	Relation * insert_cmd(string &cmd);
	//Relation * delete_cmd(string &cmd);

	void show_vector(vector<string> &v, string title);
	void remove_quotes(vector<string> &v);
	void set_database(Database *database);
};

#endif