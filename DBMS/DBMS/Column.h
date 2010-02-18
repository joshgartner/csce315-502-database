#ifndef _COLUMN_H
#define _COLUMN_H

#include <string>
#include <vector>

using namespace std;

//both need to be length 7
const string VARCHAR_ID = "VARCHAR";
const string INTEGER_ID = "INTEGER";

enum ColumnType
{
	int_type,
	string_type
};

class Column
{
public:
	string name;
	ColumnType type;
	vector<string> data;
	bool b_primary_key;

	//Column(string strName,ColumnType column_type, vector<string> strings);
	Column(string str_name, string type_id, bool b_is_primary_key);
	//Column(string strName, vector<int> ints);
	//Column(string strName, ColumnType columnType);

	//void AddItem(Item item);
	void add_items(vector<string> items);
	void add_item(string item);
	string to_string();

	int get_length(){ return data.size(); }
	string get_item(int index);
	vector<string> get_items();
	vector<string> get_items(int start, int end);
	
};

#endif