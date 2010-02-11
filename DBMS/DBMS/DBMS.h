#ifndef _DBMS_H
#define _DBMS_H

#include "Table.h"

class DBMS
{
	vector<Table> tables;

	//ctor
	DBMS();

	void AddTable(Table newTable);

	//TODO: add more accessors

};

#endif
