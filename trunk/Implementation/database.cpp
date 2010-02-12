#include "database.hpp"

/* FIXME: Constructor
*/
Database::Database(){}

/* FIXME: Deconstructor
*/
Database::~Database(){}

/* FIXME: Parses command query to create a new relation
*/
Relation Database::execute(string command){
	Relation relation;
	return relation;
}

/* FIXME: Pulls relation "name", parses the query, and performs operations.
*/
Relation Database::execute(string name, string query){
	Relation relation;
	return relation;
}       

/* FIXME: Not sure what the return type of this should be  

void Database::execute(string name, string query, back_inserter){
}*/  

/* FIXME: Adds to currently managed relations vector
*/
void Database::add_relation(Relation relation){
}

/* FIXME: Goes through relations vector and pulls out the one requested
*/
Relation Database::get_relation(string name){
	Relation r;
	return r;
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

