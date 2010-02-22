#include "database.hpp"

/*
File   : database.cpp
Authors: Darren White, John Wesson, Michael Atkinson
Team   : Team X
*/

/* Constructor:
	Sets up the database handle to the parser.
*/
Database::Database() : parser(new Parser()){
	parser->set_database(this);
}

/* Deconstructor:
	First, if a relation is flagged as saveable (by using the create command), that
	relation is saved to a file.  Then goes through relations vector and deletes 
	all the relations created by this database along with the parser.
*/
Database::~Database(){
	vector<Relation *>::iterator it;
	for(it = relations.begin(); it < relations.end(); it++){
		if((*it)->b_save == true){
			save(*it);  // This relation has been marked to be persistent
		}
	}
	clear_relations();
	delete parser;
}

/* execute(string command):
	Creates and displays a relation query for the user.
	Will show an error message if something goes wrong.
*/
void Database::execute(string query){
	Relation *r = new Relation();
	parser->remove_semicolons(query);
	r = parser->match(query);
	add_relation(r);
	r->display();
	
	/*try{
		r = parser->match(query);
		add_relation(r);
		r->display();
	}catch(exception& e){
		cout << e.what() << "\n";
	}*/
} 

/* execute(string name, string query, vector<vector<string> > &result):
	Returns to the user a vector<vector<string> > of tuples, so they can
	manipulate the data as they choose.
	Shows an error message if something goes wrong.
*/
void Database::execute(string query, vector<vector<string> > &result){
	Relation *r = new Relation();
	parser->remove_semicolons(query);
	try{
		r = parser->match(query);
		add_relation(r);
		result = r->get_items_for_user();
	}catch(exception& e){
		cout << e.what() << "\n";
	}
}

/* add_relation(Relation *r):
	Adds a relation to the currently managed relations vector.
    First searches and make sure it doesn't already exist.
    If so, just overwrite it in the vector.
*/
void Database::add_relation(Relation *r){
	vector<Relation *>::iterator it;
	for(it = relations.begin(); it < relations.end(); it++){
		if(r->name == (*it)->name){
			*it = r;
			return;
		}
	}
	relations.push_back(r);
}

/* get_relation(string name):
	Goes through relations vector and pulls out the one requested
    Throws an error if it doesn't exist.
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
	string msg = "\n **Relation " + name + " doesn't exist";
	throw Error(msg);
}

/* clear_relations():
	Called by the destructor to clear out the relations vector
*/
void Database::clear_relations(){
	vector<Relation *>::iterator it;
	for(it = relations.begin(); it < relations.end(); it++){
		delete *it;
	}
	relations.clear();
}

/* save(Relation * r):
	Writes given relation to a text file of the same name, overwrites it if already exists
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

/* load(string table_name):
	Grabs relation from file, and puts it into the database's relations vector
*/
void Database::load(string table_name){
	fstream fs(table_name.c_str(), ios::in);
	Relation * new_table = new Relation();
	new_table->name = table_name;
	new_table->b_save = true;

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

/* insert(Relation *r, Relation *from_relation):
	Inserts all the tuples from the from_relation into the r relation
*/
Relation * Database::insert(Relation *to_relation, Relation *from_relation){
	if(to_relation->columns.size() != from_relation->columns.size()){
		throw Error("\n **Error during insert: relations must have the same number of attributes");
	}
	for(int i = 0; i < from_relation->size(); i++){
		to_relation->add_tuple(from_relation->get_tuple(i));
	}
	return to_relation;
}

/* project(string str_name, Relation *from_relation, vector<string> columns):
	Creates a new relation with only certain attributes from the old relation and returns it
*/
Relation * Database::project(string str_name, Relation *from_relation, vector<string> columns){
       //copy everything from from_relation to to_relation at first
       Relation* to_relation = new Relation(from_relation);
       to_relation->b_save = false;
       to_relation->name = str_name;

       int n_columns = (int) to_relation->columns.size();

       //then delete the attributes that are not in columns
       for (int i = 0; i < n_columns; i++){
           if (to_relation->index_of(columns, to_relation->columns[i].get_name()) == -1){
               to_relation->remove_column(i);
               //we removed a column, so the index and size should get reduced by 1
               n_columns--;
               i--;
           }
       }
       return to_relation;
}

/* rename(string str_name, Relation *from_relation, vector<string> attr_list):
	Creates a new relation with from_relations columns renamed to the names in attr_list.
	Return: The new relation named "str_name"
*/
Relation * Database::rename(string str_name, Relation *from_relation, vector<string> attr_list){
	if(attr_list.size() != from_relation->columns.size()){
		throw Error("\n **Error during rename: must rename every attribute");
	}
	Relation *r = new Relation(from_relation); // Copy the relation to rename
	r->name = str_name;  // Set the name
	for(int i = 0; i < (int) attr_list.size(); i++){
		r->columns[i].name = attr_list[i];
	}
	return r;
}

/* union_of(string str_name, Relation *r1, Relation *r2):
	Performs the set operation union on r1 and r2, and returns the result.
	If r1 and r2 are not union-compatible, throws an Error
	Return: The new relation
*/
Relation * Database::union_of(string str_name, Relation *r1, Relation *r2){
   if(!r1->is_union_compatible(r2)){
	   throw Error("Relations must be union-compatible for union.");
   }

   //start with the same data as r1
   Relation* new_relation = new Relation(r1);
   new_relation->b_save = false;
   new_relation->name = str_name;

   //then add the data from r2
   for(int i = 0; i < r2->size(); i++){
       new_relation->add_tuple(r2->get_tuple(i));
   }

   //now eliminate duplicate tuples(go backwards to avoid iterator misalignment)
   for(int i = r1->size() - 1; i > -1; i--){
       for(int j = 0; j < i; j++){
		   vector<string> tuple1 = new_relation->get_tuple(i);
		   vector<string> tuple2 = new_relation->get_tuple(j);
		   bool b_equivalent = true;

		   //compare tuple1 to tuple2, if they are equal, remove tuple1 from end of relation
		   for(int k = 0; k < (int) tuple1.size(); k++){
			   if(tuple1[k] != tuple2[k]){
				   b_equivalent = false;
			   }
		   }
		   if(b_equivalent){
			   new_relation->remove_tuple(i);
			   break;
		   }
       }
   }
   return new_relation;
}

/* difference(string str_name, Relation *r1, Relation *r2):
	Performs set difference operation on r1 and r2 and returns the result. If
	r1 and r2 are not union-compatible, throws an Error
*/
Relation * Database::difference(string str_name, Relation *r1, Relation *r2){
   if(!r1->is_union_compatible(r2)){
       throw Error("Relations must be union-compatible for difference.");
   }

   //start with the same data as r1
   Relation* new_relation = new Relation(r1);
   new_relation->b_save = false;
   new_relation->name = str_name;

   //then check the data from r2, if a tuple exists in new_relation and r2, remove it from new_relation, if it does not, add it to new_relation
   for(int i = 0; i < r2->size(); i++){
       vector<string> tuple1 = r2->get_tuple(i);
       bool b_in_set = false;
       int index = -1;

       //check tuple1 against all tuples currently in new_relation
       for(int j = 0; j < (int)new_relation->size(); j++){
           vector<string> tuple2 = new_relation->get_tuple(j);
           bool b_equivalent = true;

           //compare tuple1 to tuple2
           for(int k = 0; k < (int) tuple1.size(); k++){
               if(tuple1[k] != tuple2[k]){
                   b_equivalent = false;
               }
           }

           if(b_equivalent){
               b_in_set = true;
               index = j;
               break;
           }
       }

       if(b_in_set){
           new_relation->remove_tuple(index);
       }
       else{
           new_relation->add_tuple(tuple1);
       }
   }
   return new_relation;
} // end difference

/* x_product(string str_name, Relation *r1, Relation *r2):
	Assembles the Cartesian product of r1 and r2 and returns the result
*/
Relation * Database::x_product(string str_name, Relation *r1, Relation *r2){
   Relation* new_relation = new Relation();
   new_relation->name = str_name;
   new_relation->b_save = false;

   //setup columns in the x-product relation
   for(int i = 0; i < (int)r1->columns.size(); i++){
       Column new_column(r1->columns[i].name, "VARCHAR", false);
       new_relation->add_column(new_column);
   }

   for(int i = 0; i < (int)r2->columns.size(); i++){
       Column new_column(r2->columns[i].name, "VARCHAR", false);
       new_relation->add_column(new_column);
   }

   //now add tuples to the x-product relation
   for (int i = 0; i < r1->size(); i++){
       for (int j = 0; j < r2->size(); j++){
           vector<string> final_tuple = r1->get_tuple(i);
           vector<string> rhs = r2->get_tuple(j);

           //add all values from rhs to final_tuple
           for (int k = 0; k < (int)rhs.size(); k++){
               final_tuple.push_back(rhs[k]);
           }

           new_relation->add_tuple(final_tuple);
       }
   }
   return new_relation;
}

/* natural_join(string name, Relation *r1, Relation *r2):
       Computes the natural join of r1 and r2 and returns the result.
*/
Relation * Database::natural_join(string str_name, Relation *r1, Relation *r2){
   Relation* new_relation = new Relation();
   new_relation->name = str_name;
   new_relation->b_save = false;

   vector<string> common_attributes;

   //setup columns in the natural join relation
   for (int i = 0; i < (int)r1->columns.size(); i++){
       Column new_column(r1->columns[i].name, "VARCHAR", false);
       new_relation->add_column(new_column);
   }

   //check if attribute in r2 matches attribute from r1, if so, this attribute
   //should be used to check for equality in natural join, if not, we need to
   //add the column to the natural join
   for (int i = 0; i < (int)r2->columns.size(); i++){
       if (new_relation->index_of(new_relation->columns, r2->columns[i].name) == -1){
           Column new_column(r2->columns[i].name, "VARCHAR", false);
           new_relation->add_column(new_column);
       }
       else{
           common_attributes.push_back(r2->columns[i].name);
       }
   }

   //now add tuples to the natural join relation
   for (int i = 0; i < r1->size(); i++){
       //get the values to compare from r1
       vector<string> lhs_values;
       for (int j = 0; j < (int)common_attributes.size(); j++){
           lhs_values.push_back(r1->columns[r1->index_of(r1->columns, common_attributes[j])].get_item(i));
       }

       for (int j = 0; j < r2->size(); j++){
           //compare the values from r2
           bool b_equivalent = true;

           //check if this tuple matches tuple from r1 on shared attributes
           for (int k = 0; k < (int)common_attributes.size(); k++){
               if (lhs_values[k] != r2->columns[r2->index_of(r2->columns, common_attributes[k])].get_item(j)){
                   b_equivalent = false;
               }
           }

           //if the two tuples have matching values on shared attributes, pair them and insert into new_relation
           if (b_equivalent){
               vector<string> final_tuple = r1->get_tuple(i);
               vector<string> rhs = r2->get_tuple(j);

               //add values from rhs to final_tuple that are not already there
				for (int k = 0; k < (int)rhs.size(); k++){
				   if (new_relation->index_of(lhs_values, rhs[k]) == -1){
					   final_tuple.push_back(rhs[k]);
				   }
				}

               new_relation->add_tuple(final_tuple);
           }
       }
   }

   return new_relation;
}



