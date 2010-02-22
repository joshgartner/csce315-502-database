#ifndef _relation_hpp_      // include file only once
#define _relation_hpp_

/*
File   : relation.hpp
Authors: Darren White, John Wesson
Team   : Team X
*/

#include "error.hpp"
#include "column.hpp"
#include <sstream>
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
	Relation(Relation *from_relation); // Deep copies a relation
    ~Relation(); // Deconstructor
    
    string name;             // Name of the relation
	bool b_save;             // If this flag is true, the relation is persistent
    vector<Column> columns;  // Representation of the relation table

	int index_of(vector<string> list, string item); // Finds "item" in list
	int index_of(vector<int> list, int item);		// Finds "item" in list (for primary keys)
	int index_of(vector<Column> list, string item); // Finds the column with attribute name "item" in columns
    
    vector<string> get_tuple(int index);            
    vector<string> get_column(int index);
	vector< vector<string> > get_items_for_user();//Returns data to fill the back_inserter to give to the user
    void add_column(Column new_column);           // Push a vector onto the back of the table
	void add_relation(Relation *r1, Relation *r2);// Add a relation to a relation, r1's name is the name of it
    void remove_column(int index);                // Remove the given attribute column from the relationo
    void add_tuple(vector<string> tuple);         // Add a tuple to the relation
    void remove_tuple(int index);                 // Removes a tuple from the relation
	void copy_attrs(Relation *r);                 // Copy this relation's columns to relation r's columns
	int size();                                   // Returns the # of tuples (rows) in the relation
    
    void display();                               // Displays a relation to the terminal
	bool is_union_compatible(Relation* r);        // Tell us if this Relation is union-compatible with the other
	int str_compare(string left, string right);   // Converts strings to integer values and compares them
	vector<bool> compare(string &attr, string &value, string &op); // Does comparisons with given op
	void update_attrs(vector<string> &attr, vector<string> &literals, int index); // Changes tuple values in the given rows
};

#endif