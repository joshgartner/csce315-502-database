#ifndef _DBMS_H
#define _DBMS_H

#include "Table.h"

class DBMS
{
	Table current_table;

public:
	//ctor
	DBMS();

	void add_table(Table new_table);

	void load_tables();
	void load(string table_name);
	//TODO: add more accessors

};

#endif
