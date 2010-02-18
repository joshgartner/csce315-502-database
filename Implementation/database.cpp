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
	}catch(exception& e){
		cout << e.what() << "\n";
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
	Relation *r;
	vector<Relation *>::iterator it;
	for(it = relations.begin(); it < relations.end(); it++){
		r = *it;
		if(r->name == name){
			return r;
		}
	}
	string msg = " **Relation " + name + " doesn't exist";
	throw Error(msg);
}

/* clear_relations(): called by the destructor to clear out the relations vector
*/
void Database::clear_relations(){
	vector<Relation *>::iterator it;
	for(it = relations.begin(); it < relations.end(); it++){
		delete *it;
	}
	relations.clear();
}

/* FIXME: Writes given relation to a text file of the same name, overwrites it if already exists
*/
void Database::save(Relation * r){
	fstream fs((r->name).c_str(), ios::out);

	fs << (r->columns).size() << "\n";
	fs << (r->columns[0]).get_length() << "\n";

	for(unsigned int i = 0; i < (r->columns).size(); i++){
		fs << (r->columns[i]).to_string() << "\n";
	}

	fs.close();
}

/* Grabs relation from file, puts into relations vector
*/
void Database::load(string table_name){
	fstream fs(table_name.c_str(), ios::in);
	Relation * new_table = new Relation();
	new_table->name = table_name;

	int num_columns;
	int num_tuples;
	string str_type;     //VARCHAR or INTEGER
	int is_primary_key;  //1 or 0
	char str_name[256];  //the name of the attribute
	char str_entry[256];

	fs >> num_columns;
	fs >> num_tuples;

	for(int i = 0; i < num_columns; i++){
		fs >> is_primary_key;
		fs >> str_type;
		fs.get();  //trash the next \n
		fs.getline(str_name, 256);

		Column new_column = Column((string)str_name, str_type, (is_primary_key == 1));

		for(int k = 0; k < num_tuples; k++){
			fs.getline(str_entry, 256);
			new_column.add_item((string)str_entry);
		}

		new_table->add_column(new_column);
	}
	add_relation(new_table);
}

// Add the row to the table, make sure the length is right!
Relation * Database::insert(Relation *r, vector<string> row){
	return r;
}

// Add all the rows to r from the given relation
Relation * Database::insert(Relation *r, Relation *from_relation){
	return r;
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



