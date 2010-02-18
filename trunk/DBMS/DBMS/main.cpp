#include <vector>
#include "DBMS.h"

int main()
{

	DBMS db = DBMS();
	vector<string> attr;
	vector<string> types;
	attr.push_back("attr1");
	attr.push_back("attr2");
	attr.push_back("attr3");
	types.push_back("VARCHAR(13)");
	types.push_back("INTEGER");
	types.push_back("VARCHAR(300)");

	vector<string> pk;
	pk.push_back("attr2");

	vector<string> tuple;
	tuple.push_back("bob");
	tuple.push_back("45");
	tuple.push_back("hard-working");

	vector<string> tuple2;
	tuple2.push_back("mary");
	tuple2.push_back("33");
	tuple2.push_back("lazy");

	Table testTable("table1", attr, types, pk);

	testTable.add_tuple(tuple);
	testTable.add_tuple(tuple2);
	db.add_table(testTable);

	testTable.display();

	while(1){}

	return 0;
	//TODO: test dbms
}