#ifndef _database_hpp_      // include file only once
#define _database_hpp_

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
    
    void execute(string command);    // Parses command query to create a new relation
    //void execute(string name, string query, back_inserter); 
    
    void add_relation(Relation *r);       // Adds to currently managed relations vector
    Relation * get_relation(string name); // Goes through relations vector and pulls out the one requested
	void clear_relations();  // Delete all the relations in the relation vector
    void save(Relation * r);   // Writes given relation to a text file of the same name
    void load(string name);  // Grabs relation from file, puts into relations vector

	Relation * insert(Relation *r, vector<string> row);      // Add the row to the table
	Relation * insert(Relation *r, Relation *from_relation); // Add all the rows of the given relation

	 /* The queries should preserve the relation provided, so always create a new one and return that.
       All of these should throw an Error exception if vectors don't match up or something.
    */
    Relation * select(string name, Relation *r, vector<string> lhs, vector<string> rhs, vector<int> ops);
        /* Input will look like "select (x1 == y1, x2 != y2) r"
           So use the vector indices to line up expressions, and pull attributes (column names)
           from the relation r.  You'll probably use a switch statement on ops.  So find lhs[0] in r, 
           and then for that whole vector, compare it to rhs[0] with the given operation.
           When the condition is true, take that row and add it to the relation you are going to return (named name).
        */
    
    Relation * project(string name, Relation *r, vector<string> attr_list);
        /* Go through r and steal all the columns with the same names found in "attr_list"  add these to another relation 
           called "name" and return it.
        */
        
    Relation * rename(string name, Relation *r, vector<string> attr_list);
        /* Attr_list is a bunch of column names corresponding to r's columns.  Rename r's columns with the ones
           provided, and return the new relation named "name" with renamed columns.
        */
        
    Relation * union_of(string name, Relation *r1, Relation *r2);
    Relation * difference(string name, Relation *r1, Relation *r2);
    Relation * x_product(string name, Relation *r1, Relation *r2);
    Relation * natural_join(string name, Relation *r1, Relation *r2);
private:
    Parser *parser;
    vector<Relation *> relations;             // All the relations database is handling currently
};

#endif
