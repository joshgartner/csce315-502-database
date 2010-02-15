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

void Relation::update_attrs(vector<string> &attr, vector<string> &literals, int index){}

/* FIXME: Input will look like "select (x1 == y1, x2 != y2) r"
   So use the vector indices to line up expressions, and pull attributes (column names)
   from the relation r.  You'll probably use a switch statement on ops.  So find lhs[0] in r, 
   and then for that whole vector, compare it to rhs[0] with the given operation.
   When the condition is true, take that row and add it to the relation you are going to return (named name).
*/
Relation * Relation::select(string name, Relation rela, vector<string> lhs, vector<string> rhs, vector<int> ops){
	Relation *r = new Relation();
	return r;
}

/* FIXME: Go through r and steal all the columns with the same names found in "columns"  add these to another relation 
   called "name" and return it.
*/
Relation * Relation::project(string name, Relation rela, vector<string> columns){
	Relation *r = new Relation();
	return r;
}

/* FIXME: Attr_list is a bunch of column names corresponding to r's columns.  Rename r's columns with the ones
   provided, and return the new relation named "name" with renamed columns.
*/
Relation * Relation::rename(string name, Relation rela, vector<string> attr_list){
	Relation *r = new Relation();
	return r;
}

/* FIXME:
*/
Relation * Relation::union_of(string name, Relation r1, Relation r2){
	Relation *r = new Relation();
	return r;
}

/* FIXME:
*/
Relation * Relation::difference(string name, Relation r1, Relation r2){
	Relation *r = new Relation();
	return r;
}

/* FIXME:
*/
Relation * Relation::x_product(string name, Relation r1, Relation r2){
	Relation *r = new Relation();
	return r;
}

/* FIXME:
*/
Relation * Relation::natural_join(string name, Relation r1, Relation r2){
	Relation *r = new Relation();
	return r;
}