#ifndef _TABLE_H
#define _TABLE_H

#include "Column.h"
#include <vector>
#include <map>

using namespace std;

class Table
{
private:
	string name;
	vector<Column> columns;

public:
	Table(string strName);
	Table(string strName, map<string, ColumnType> attributeList);
	Table(string strName, vector<Column> columns);

	void AddColumn(Column newColumn);
	//TODO: finish accessor functions
	// and add implementation
};

#endif
