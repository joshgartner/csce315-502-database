#include "column.hpp"
#include "error.hpp"

/*
File   : column.cpp
Authors: John Wesson
Team   : Team X
*/

#include <iostream>  // For debug
using std::cout;

/* Column(string str_name, string type_id, bool b_is_primary_key):
	constructor that sets name to str_name and b_primary_key to b_is_primary_key
*/
Column::Column(string str_name, string type_id, bool b_is_primary_key){
	name = str_name;
	b_primary_key = b_is_primary_key;
}

/* Column(Column* from_column):
	copy constructor(deep copy)
*/
Column::Column(Column* from_column){
       name = from_column->name;
       b_primary_key = from_column->b_primary_key;
       type = from_column->type;
       data = from_column->data;
}

/* to_string():
	constructs a string from the various data associated with column
	so that it can be written to a file
*/
string Column::to_string(){
	string str_column = (b_primary_key)? "1\n" : "0\n";
	str_column += (type == int_type)? INTEGER_ID : VARCHAR_ID;
	str_column += "\n" + name;

	for (unsigned int i = 0; i < data.size(); i++){
		str_column += "\n" + data[i];
	}

	return str_column;
}

/* add_item(string item):
	adds an item to the end of the column
	Throws an Error if this attribute is a primary key and item
	already exists in the Column
*/
void Column::add_item(string item){
   if((b_primary_key) && contains(item)){
       throw Error("\n **Cannot have duplicate values under primary key.");
   }
   data.push_back(item);
}

/* get_item(int index):
	returns the item at index
*/
string Column::get_item(int index){
	return data[index];
}

/* set_item(int index, string item):
	overwrites the value at index to item
*/
void Column::set_item(int index, string item){
	data[index] = item;
}

/* get_items():
	returns all of the values in the Column
*/
vector<string> Column::get_items(){
	return data;
}

/* contains(string item):
	returns true if the column contains item, otherwise false
*/
bool Column::contains(string item){
   bool b_contains = false;

   for(int i = 0; i < (int) data.size(); i++){
       if(item == data[i]){
           b_contains = true;
       }
   }
   return b_contains;
}

/* remove_item(int index):
	removes the item at index from Column
*/
void Column::remove_item(int index){
	data.erase(data.begin() + index);
}

/* remove_items():
	clears all of the items from the Column
*/
void Column::remove_items(){
	data.clear();
}
