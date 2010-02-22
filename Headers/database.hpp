#ifndef _database_hpp_      // include file only once
#define _database_hpp_

/*
File   : database.hpp
Authors: Darren White, John Wesson, Michael Atkinson
Team   : Team X
*/

#include <iostream>
#include <string>
#include <vector>
#include "relation.hpp"
#include "parser.hpp"
#include "column.hpp"
#include "error.hpp"

// Forward Declaration
class Parser;

using namespace std;

class Database{
public:
    Database();   // Constructor
    ~Database();  // Deconstructor
    
    void execute(string command);         // Parses command query to create a new relation
    void execute(string query, vector<vector<string> > &result); // Sets result matrix to data according to query
    
    void add_relation(Relation *r);       // Adds to currently managed relations vector
    Relation * get_relation(string name); // Goes through relations vector and pulls out the one requested
	void clear_relations();               // Delete all the relations in the relation vector
    void save(Relation * r);              // Writes given relation to a text file of the same name
    void load(string name);               // Grabs relation from file, puts into relations vector

	Relation * insert(Relation *r, Relation *from_relation); // Add all the rows of the given relation

	// Query operations:
    Relation * project(string str_name, Relation *from_relation, vector<string> attr_list);
    Relation * rename(string str_name, Relation *from_relation, vector<string> attr_list);
    Relation * union_of(string str_name, Relation *r1, Relation *r2);
    Relation * difference(string str_name, Relation *r1, Relation *r2);
    Relation * x_product(string str_name, Relation *r1, Relation *r2);
    Relation * natural_join(string str_name, Relation *r1, Relation *r2);
private:
    Parser *parser;                // A parser for parsing input commands in execute
    vector<Relation *> relations;  // All the relations database is handling currently
};

#endif
