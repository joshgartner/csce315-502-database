#ifndef _database_hpp_      // include file only once
#define _database_hpp_

#include <iostream>
#include <string>
#include <vector>

class Database{
public:
    Database();   // Constructor
    ~Database();  // Deconstructor
    
    Relation execute(string command);                  // Parses command query to create a new relation
    Relation execute(string name, string query);       // Pulls relation "name", parses the query, and performs operations.
    
    void execute(string name, string query, back_inserter);  // Not sure what the return type of this should be  
    
    void add_relation(Relation r);                     // Adds to currently managed relations vector
    Relation get_relation(string name);                // Goes through relations vector and pulls out the one requested
    bool save(Relation r);                             // Writes given relation to a text file of the same name, (warn/overwrite if already exists?)
    bool load(string name);                            // Grabs relation from file, puts into relations vector, returns true if successful
private:
    Parser p;
    vector<Relation> relations;  // All the relations database is handling currently, perhaps we should limit its size?
};

#endif
