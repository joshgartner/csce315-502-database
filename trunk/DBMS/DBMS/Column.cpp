#include "Column.h"

Column::Column(string strName, vector<string> strings)
{
	name = strName;
	type = StringType;

	for (unsigned int i = 0; i < strings.size(); i++)
	{
		Item tempItem;
		tempItem.strItem = strings[i];

		data.push_back(tempItem);
	}
}

Column::Column(string strName, vector<int> ints)
{
	name = strName;
	type = IntType;

	for (unsigned int i = 0; i < ints.size(); i++)
	{
		Item tempItem;
		tempItem.intItem = ints[i];

		data.push_back(tempItem);
	}
}

Column::Column(string strName, ColumnType columnType)
{
	name = strName;
	type = columnType;
}

void Column::AddItem(Item item)
{
	data.push_back(item);
}

void Column::AddItems(vector<Item> items)
{
	for(unsigned int i = 0; i < items.size(); i++)
	{
		data.push_back(items[i]);
	}
}

Item Column::GetItem(int index)
{
	return data[index];
}

vector<Item> Column::GetItems()
{
	return data;
}

vector<Item> Column::GetItems(int start, int end)
{
	vector<Item> returnItems;

	for (int i = start; i < end; i++)
	{
		returnItems.push_back(data[i]);
	}

	return returnItems;
}