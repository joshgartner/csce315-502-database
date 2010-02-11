#ifndef _COLUMN_H
#define _COLUMN_H

#include "Item.h"
#include <string>
#include <vector>

using namespace std;

enum ColumnType
{
	IntType,
	StringType
};

class Column
{
private:
	string name;
	ColumnType type;
	vector<Item> data;

public:
	Column(string strName, vector<string> strings);
	Column(string strName, vector<int> ints);
	Column(string strName, ColumnType type);

	void AddItem(Item item);
	void AddItems(vector<Item> items);

	Item GetItem(int index);
	vector<Item> GetItems();
	vector<Item> GetItems(int start, int end);
	
};

#endif