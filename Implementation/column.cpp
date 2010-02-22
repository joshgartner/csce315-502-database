#include "column.hpp"
#include "error.hpp"

/*
File   : column.cpp
Authors: John Wesson
Team   : Team X
*/

#include <iostream>  // For debug
using std::cout;

Column::Column(string str_name, string type_id, bool b_is_primary_key){
	name = str_name;

	if (type_id.compare(VARCHAR_ID) == 0){
		type = string_type;
	}
	else if(type_id.compare(INTEGER_ID) == 0){
		type = int_type;
	}
	else{
		cout << "Type id was: " << type_id << "\n";
		throw Error(" **Invalid type found, types are: VARCHAR or INTEGER");
	}
	b_primary_key = b_is_primary_key;
}

Column::Column(Column* from_column){
       name = from_column->name;
       b_primary_key = from_column->b_primary_key;
       type = from_column->type;
       data = from_column->data;
}

string Column::to_string(){
	string str_column = (b_primary_key)? "1\n" : "0\n";
	str_column += (type == int_type)? INTEGER_ID : VARCHAR_ID;
	str_column += "\n" + name;

	for (unsigned int i = 0; i < data.size(); i++){
		str_column += "\n" + data[i];
	}

	return str_column;
}

void Column::add_item(string item){
   if ((b_primary_key) && contains(item)){
       throw Error("\n **Cannot have duplicate values under primary key.");
   }
   data.push_back(item);
}

void Column::add_items(vector<string> items){
   for(unsigned int i = 0; i < items.size(); i++){
       add_item(items[i]);
   }
}

string Column::get_item(int index){
	return data[index];
}

//sets the value at index to item
void Column::set_item(int index, string item){
	data[index] = item;
}

vector<string> Column::get_items(){
	return data;
}

bool Column::contains(string item){
       bool b_contains = false;

       for (int i = 0; i < (int) data.size(); i++){
               if (item == data[i]){
                       b_contains = true;
               }
       }

       return b_contains;
}

vector<string> Column::get_items(int start, int end){
	vector<string> return_items;

	for (int i = start; i < end; i++){
		return_items.push_back(data[i]);
	}

	return return_items;
}

void Column::remove_item(int index){
	data.erase(data.begin() + index);
}

void Column::remove_items(){
	data.clear();
}
