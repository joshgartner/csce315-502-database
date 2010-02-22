#ifndef _column_hpp
#define _column_hpp

/*
File   : column.hpp
Authors: John Wesson
Team   : Team X
*/

#include <string>
#include <vector>

using namespace std;


class Column{
private:
	vector<string> data;											//the values for this column
public:
	Column(string str_name, string type_id, bool b_is_primary_key); //constructor
	Column(Column* from_column);									//copy constructor

	string name;													//attribute name
	bool b_primary_key;												//is this attribute a primary key?

	void add_item(string item);										//add item to column
	void set_item(int index, string item);							//set value at index to item
	void set_name(string new_name){ name = new_name; }				//rename the attribute

	// Accessors
	int get_length(){ return data.size(); }							//return length of column
	string get_name(){ return name; }								//return attribute name
	string get_item(int index);										//return value at index
	vector<string> get_items();										//return all values in column

	void remove_item(int index);									//remove item at index
	void remove_items();											//remove all items from column

	bool contains(string item);										//does this column contain item?
	string to_string();												//used in saving to disk
};

#endif