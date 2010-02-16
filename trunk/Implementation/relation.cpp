#include "relation.hpp"

/* FIXME: Create empty relation (otherwise known as a table)
*/
Relation::Relation(){}

/* FIXME: Constructor - Create relation with given data
*/
Relation::Relation(string name, vector<string> column_names, vector<string> types, vector<string> primary_keys){}

/* FIXME: Deconstructor
*/
Relation::~Relation(){}

vector<string> Relation::get_row(int index){
	vector<string> dummy(1);
	return dummy;
}

vector<string> Relation::get_column(int index){
	vector<string> dummy(1);
	return dummy;
}

/* FIXME: Push a vector onto the back of the table
*/
void Relation::add_column(vector<string> new_column){
}

/* FIXME: Remove the given column, might take iterator
*/
void Relation::remove_column(int index){
}

/* FIXME: tuple[0] gets pushed onto table[0]'s vector, tuple[1] onto table[1]'s, etc
*/
void Relation::add_row(vector<string> tuple){
}

/* FIXME: (arg could be iterator), remove the same index from each column.
*/
void Relation::remove_row(int index){
}

/* FIXME: Return the size of the table.  Length of one of the vectors basically
*/
int Relation::size(){
	return 10;
}

/* FIXME:
*/
void Relation::display(){
}        

/* FIXME:
*/
int Relation::compare(string &attr, string &entry, string &op){
	int index = 0;
	// Find the index of the row that matches and return it
	// Return 0, if nothing matches.
	return index;
}

/* FIXME:
*/
void Relation::update_attrs(vector<string> &attr, vector<string> &literals, int index){
}
