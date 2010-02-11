#include <string>
#include <vector>

#include "DBMS.h"

DBMS::DBMS()
{
	//TODO: add ctor
}

void DBMS::AddTable(Table newTable)
{
	tables.push_back(newTable);
}
