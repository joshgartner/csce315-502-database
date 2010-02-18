#ifndef _TABLE_H
#define _TABLE_H

#include "Column.h"
#include <vector>
#include <map>

using namespace std;

class Table
{
public:

	string name;
	vector<Column> columns;

	Table(){}//default ctor
	Table(string str_name, vector<string> attributes, vector<string> types, vector<string> primary_keys);

	int index_of(vector<string> list, string item);
	int index_of(vector<int> list, int item);

	void add_tuple(vector<string> tuple);

	void save();

	void add_column(Column new_column);
	void display();
};

#endif
