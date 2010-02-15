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

