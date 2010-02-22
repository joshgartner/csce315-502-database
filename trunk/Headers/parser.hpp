#ifndef _parser_hpp_      // include file only once
#define _parser_hpp_

/*
File   : parser.hpp
Authors: Darren White
Team   : Team X
*/

#include "relation.hpp"
#include "database.hpp"
#include "error.hpp"
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

// Forward Declarations
class Database;

using namespace boost::xpressive;
using std::vector;
using std::string;
using std::cout;

enum { CREATE, UPDATE, INSERT, DELETE, SELECT, PROJECT, RENAME, EXPR };

/* class Parser:
	Given commands and queries as input, the Parser class evaluates them for
	correctness and calls the appropriate database commands if necessary.  Most functions
	return the new relation to the database.  The Parser makes heavy use of the
	boost::xpressive library for matching/tokenizing regular expressions.
	Invariants:
		- No function should change the database handle except for the database.
		- Functions return a valid and useable relation to the database or throw an error
*/
class Parser{
public:
	Parser();

	Database *dbms;                // Handle to the database
	sregex id, literals, op, type; // String regular expressions
	sregex query_op, atomic_expr;
	int n_temps;                   // Number of temporary relations created, used so temp names don't overlap.

	Relation * match(string &input); // The entry point, selects appropriate cmd or query
	
	Relation * create_cmd(string &cmd); // Creates a new relation
	Relation * insert_cmd(string &cmd); // Insert values into a relation, or from a relation
	Relation * update_cmd(string &cmd); // Change values in rows of a relation
	Relation * delete_cmd(string &cmd); // Deletes rows from a relation based on a condition

	Relation * select_query(string &cmd);  // Selects rows based on a condition
	Relation * project_query(string &cmd); // Takes attributes (columns) of a relation
	Relation * rename_query(string &cmd);  // Renames attributes in a relation
	
	// expr_query handles union, difference, x_product, and natural join
	Relation * expr_query(string &name, Relation *r1, Relation *r2, string operation);
	vector<bool> condition(Relation *r, string input); // Finds rows that match a condition
	
	string temp_name(); // Generate a temporary name for a relation
	void show_vector(vector<string> &v, string title); // Display a vector
	void remove_quotes(string &s);         // Remove quotes from a vector
	void remove_quotes(vector<string> &v); // Remove quotes from a vector of strings
	void remove_semicolons(string &s);     // Remove semicolons from a string
	void set_database(Database *database); // Establish the handle to the databse
};

#endif