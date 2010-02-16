#ifndef _relation_hpp_      // include file only once
#define _relation_hpp_

#include "error.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum {EQ, NEQ, LT, GT, LTE, GTE};

class Relation{
public:
    Relation();  // Create empty relation (otherwise known as a table)
    Relation(string name, vector<string> column_names, vector<string> types, vector<string> primary_keys);  // Create relation with given data
    ~Relation(); // Deconstructor
    
    string name;
    vector<string> primary_keys;
    vector<string> types;
    
    /* Not sure how you wanted to make the table, this is probably not the best way:  I dunno */
    vector<vector<string>> table;
    
    // These are all needed to make database queries easier (a tuple is just a fancy name for a row):
    vector<string> get_row(int index);            // Or maybe an iterator as argument
    vector<string> get_column(int index);
    void add_column(vector<string> new_column);   // Push a vector onto the back of the table
	void add_relation(Relation *r1, Relation *r2);// Add a relation to a relation, r1's name is the name of it
    void remove_column(int index);                // Remove the given column, might take iterator
    void add_row(vector<string> tuple);           // tuple[0] gets pushed onto table[0]'s vector, tuple[1] onto table[1]'s, etc
    void remove_row(int index);                   // (or iterator), remove the same index from each column.
	int size();                                   // Returns the # of rows in the table, length of vector basically
    
    void display();                               // Show us the table!  For loops and couts.

	void compare(string &attr, string &entry, string &op, vector<bool> &matches);
		/* Do the comparison in the relation, update matches to reflect true for any rows that the condition is true for.
		   An expression is:  attr op entry.  So look in the attr column and compare all the values
		   to "op entry", so like age < 7 would look in the age column, and set all the indexes in matches to true
		   where the value was < 7.
        */

	void update_attrs(vector<string> &attr, vector<string> &literals, int index);
		/* Go to the index row of the relation, change the corresponding attrs to the
		   literals provided.
        */
};

#endif