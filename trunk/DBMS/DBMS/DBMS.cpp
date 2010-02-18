#define TABLE_LIST_FILE "Tables.l"

#include <fstream>
#include <string>
#include <vector>

#include "DBMS.h"

DBMS::DBMS()
{
	//TODO: add ctor
	load_tables();
}

void DBMS::add_table(Table new_table)
{
	new_table.save();
	load(new_table.name);
}

void DBMS::load_tables()
{

}

void DBMS::load(string table_name)
{
	fstream fs(table_name.c_str(), ios::in);
	Table new_table = Table();
	new_table.name = table_name;

	int num_columns;
	int num_tuples;
	string str_type;//VARCHAR or INTEGER
	int is_primary_key;//1 or 0
	char str_name[256];//the name of the attribute
	char str_entry[256];

	fs >> num_columns;
	fs >> num_tuples;

	for( int i = 0; i < num_columns; i++)
	{
		fs >> is_primary_key;
		fs >> str_type;
		fs.get();//trash the next \n
		fs.getline(str_name, 256);

		Column new_column = Column((string)str_name, str_type, (is_primary_key == 1));

		for(int k = 0; k < num_tuples; k++)
		{
			fs.getline(str_entry, 256);
			new_column.add_item((string)str_entry);
		}

		new_table.add_column(new_column);
	}

	current_table = new_table;
}
