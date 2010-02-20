#ifndef _parser_hpp_      // include file only once
#define _parser_hpp_

#include "relation.hpp"
#include "database.hpp"
#include "error.hpp"
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

	Database *dbms; // Handle to the database
	sregex id, literals, op, type; // Regexes for terminals
	sregex query_op, atomic_expr;

	Relation * match(string &input); 
	vector<bool> condition(Relation *r, string input);
	Relation * create_cmd(string &cmd);
	Relation * insert_cmd(string &cmd);
	Relation * update_cmd(string &cmd);
	Relation * delete_cmd(string &cmd);

	Relation * select_query(string &cmd);
	Relation * project_query(string &cmd);
	Relation * rename_query(string &cmd);
	
	Relation * expr_query(string &name, Relation *r1, Relation *r2, string operation);
	
	void show_vector(vector<string> &v, string title);
	void remove_quotes(string &s);
	void remove_quotes(vector<string> &v);
	void remove_semicolons(string &s);
	void set_database(Database *database);
};

#endif