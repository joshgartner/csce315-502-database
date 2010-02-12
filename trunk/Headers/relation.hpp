#ifndef _relation_hpp_      // include file only once
#define _relation_hpp_

#include <iostream>
#include <string>
#include <vector>

enum {EQ, NEQ, LT, GT, LTE, GTE};

class Relation{
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
    void remove_column(int index);                // Remove the given column, might take iterator
    void add_row(vector<string> tuple);           // tuple[0] gets pushed onto table[0]'s vector, tuple[1] onto table[1]'s, etc
    void remove_row(int index);                   // (or iterator), remove the same index from each column.
    
    void display();        // Show us the table!  For loops and couts.
    
    /* The queries should preserve the relation provided, so always create a new one and return that.
       All of these should return NULL if things go bad, like vectors don't match up or something.
    */
    Relation select(string name, Relation r, vector<string> lhs, vector<string> rhs, vector<int> ops);
        /* Input will look like "select (x1 == y1, x2 != y2) r"
           So use the vector indices to line up expressions, and pull attributes (column names)
           from the relation r.  You'll probably use a switch statement on ops.  So find lhs[0] in r, 
           and then for that whole vector, compare it to rhs[0] with the given operation.
           When the condition is true, take that row and add it to the relation you are going to return (named name).
        */
    
    Relation project(string name, Relation r, vector<string> columns);
        /* Go through r and steal all the columns with the same names found in "columns"  add these to another relation 
           called "name" and return it.
        */
        
    Relation rename(string name, Relation r, vector<string> attr_list);
        /* Attr_list is a bunch of column names corresponding to r's columns.  Rename r's columns with the ones
           provided, and return the new relation named "name" with renamed columns.
        */
        
    Relation union_of(string name, Relation r1, Relation r2);     // You'll have to look these up
    Relation difference(string name, Relation r1, Relation r2);
    Relation x_product(string name, Relation r1, Relation r2);
    Relation natural_join(string name, Relation r1, Relation r2);
};

#endif