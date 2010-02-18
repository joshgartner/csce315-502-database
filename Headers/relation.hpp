#ifndef _relation_hpp_      // include file only once
#define _relation_hpp_

#include "error.hpp"
#include "column.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

enum {EQ, NEQ, LT, GT, LTE, GTE};

class Relation{
public:
    Relation();  // Create empty relation (otherwise known as a table)
    Relation(string str_name, vector<string> attributes, vector<string> types, vector<string> primary_keys);
    ~Relation(); // Deconstructor
    
    string name;                  // Name of the relation
    vector<string> primary_keys;
    vector<string> types;
    
    vector<Column> columns;
	int index_of(vector<string> list, string item);
	int index_of(vector<int> list, int item);
    
    vector<string> get_row(int index);            // Or maybe an iterator as argument
    vector<string> get_column(int index);
    void add_column(Column new_column);           // Push a vector onto the back of the table
	void add_relation(Relation *r1, Relation *r2);// Add a relation to a relation, r1's name is the name of it
    void remove_column(int index);                // Remove the given column, might take iterator
    void add_tuple(vector<string> tuple);         
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