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

	Relation * match(string &input); 
	void condition(Relation *r, string input, vector<bool> &matches);
	Relation * create_cmd(string &cmd);
	Relation * insert_cmd(string &cmd);
	Relation * update_cmd(string &cmd);
	Relation * delete_cmd(string &cmd);

	void show_vector(vector<string> &v, string title);
	void remove_quotes(vector<string> &v);
	void set_database(Database *database);
};

#endif