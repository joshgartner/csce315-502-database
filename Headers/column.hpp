#ifndef _column_hpp
#define _column_hpp

#include <string>
#include <vector>

using namespace std;

//both need to be length 7
const string VARCHAR_ID = "VARCHAR";
const string INTEGER_ID = "INTEGER";

enum column_type { int_type, string_type };

class Column{
private:
	string name;
	column_type type;
	vector<string> data;
	bool b_primary_key;

public:
	//Column(string strName,ColumnType column_type, vector<string> strings);
	Column(string str_name, string type_id, bool b_is_primary_key);
	//Column(string strName, vector<int> ints);
	//Column(string strName, ColumnType columnType);

	//void AddItem(Item item);
	void add_items(vector<string> items);
	void add_item(string item);
	void set_item(int index, string item);
	string to_string();

	// Accessors
	int get_length(){ return data.size(); }
	string get_name(){ return name; }
	bool isPrimary(){ return b_primary_key; }
	void set_name(string new_name){ name = new_name; }

	string get_item(int index);
	vector<string> get_items();
	vector<string> get_items(int start, int end);

	void remove_item(int index);
	void remove_items();
};

#endif