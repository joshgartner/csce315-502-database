#include "database.hpp"

/* FIXME: Constructor
*/
Database::Database() : parser(new Parser()){
	parser->set_database(this);
}

/* FIXME: Deconstructor
*/
Database::~Database(){
	clear_relations();
	delete parser;
}

/* FIXME: Parses command query to create a new relation
*/
void Database::execute(string command){
	Relation *r = new Relation();
	try{
		r = parser->match(command);
		add_relation(r);
	}catch(Error e){
		cout << e.message;
	}
} 

/* FIXME: Not sure what the return type of this should be  

void Database::execute(string name, string query, back_inserter){
}*/  

/* FIXME: Adds to currently managed relations vector
   Needs to first search and make sure it doesn't already exist.
   If so, just overwrite it in the vector.
*/
void Database::add_relation(Relation *r){
	relations.push_back(r);
}

/* FIXME: Goes through relations vector and pulls out the one requested
   Should throw a "Relation not found error" if it doesn't exist.
*/
Relation * Database::get_relation(string name){
	Relation *r = new Relation();
	return r;
}

/* clear_relations(): called by the destructor to clear out the relations vector
*/
void Database::clear_relations(){
	vector<Relation *>::iterator it;
	for(it = relations.begin() ; it < relations.end(); it++){
		delete *it;
	}
	relations.clear();
}

/* FIXME: Writes given relation to a text file of the same name, (warn/overwrite if already exists?)
*/
bool Database::save(Relation relation){
	return true;
}

/* FIXME: Grabs relation from file, puts into relations vector, returns true if successful
*/
bool Database::load(string name){
	return true;
}

/* FIXME: Input will look like "select (x1 == y1, x2 != y2) r"
   So use the vector indices to line up expressions, and pull attributes (column names)
   from the relation r.  You'll probably use a switch statement on ops.  So find lhs[0] in r, 
   and then for that whole vector, compare it to rhs[0] with the given operation.
   When the condition is true, take that row and add it to the relation you are going to return (named name).
*/
Relation * Database::select(string name, Relation *r, vector<string> lhs, vector<string> rhs, vector<int> ops){
	return r;
}

/* FIXME: Go through r and steal all the columns with the same names found in "columns"  add these to another relation 
   called "name" and return it.
*/
Relation * Database::project(string name, Relation *r, vector<string> columns){
	return r;
}

/* FIXME: Attr_list is a bunch of column names corresponding to r's columns.  Rename r's columns with the ones
   provided, and return the new relation named "name" with renamed columns.
*/
Relation * Database::rename(string name, Relation *r, vector<string> attr_list){
	return r;
}

/* FIXME:
*/
Relation * Database::union_of(string name, Relation *r1, Relation *r2){
	return r1;
}

/* FIXME:
*/
Relation * Database::difference(string name, Relation *r1, Relation *r2){
	return r1;
}

/* FIXME:
*/
Relation * Database::x_product(string name, Relation *r1, Relation *r2){
	return r1;
}

/* FIXME:
*/
Relation * Database::natural_join(string name, Relation *r1, Relation *r2){
	return r1;
}



