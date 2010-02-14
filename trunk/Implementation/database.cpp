#include "database.hpp"

/* FIXME: Constructor
*/
Database::Database() : parser(new Parser()){
	parser->set_database(this);
}

/* FIXME: Deconstructor
*/
Database::~Database(){
	delete parser;
}

/* FIXME: Parses command query to create a new relation
*/
void Database::execute(string command){
	parser->match(command);
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
Relation * Database::get_relation(string name){
	Relation *r = new Relation();
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

