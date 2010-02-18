#include "relation.hpp"

/* FIXME: Create empty relation (otherwise known as a table)
*/
Relation::Relation(){}

/* Constructor - Create relation with given data
*/
Relation::Relation(string str_name, vector<string> attributes, vector<string> types, vector<string> primary_keys){
	name = str_name;
	vector<int> primary_key_indices;
	bool b_valid_data = true;

	if (attributes.size() != types.size()){
		b_valid_data = false;
	}

	//if we got data, we must have at least one primary key
	if ((attributes.size() > 0) && (primary_keys.size() == 0)){
		b_valid_data = false;
	}

	//make sure each primarykey is in attributes
	for (unsigned int i = 0; i < primary_keys.size(); i++){
		int index = index_of(attributes, primary_keys[i]);

		//if primary key is in attributes, remember index so we can store that in column data
		if (index != -1){
			primary_key_indices.push_back(index);
		}
		//if it is not, throw an error
		else{
			b_valid_data = false;
			break;
		}
	}

	vector<Column> list_columns;

	//make column from each attribute and add to columns
	for (unsigned int i = 0; i < attributes.size(); i++){
		string type_id = types[i].substr(0, 7);

		//decide if this attribute is a primary key
		bool b_is_primary_key = (index_of(primary_key_indices, i) == -1);

		Column new_column(attributes[i], type_id, b_is_primary_key);
		list_columns.push_back(new_column);
	}

	if (!b_valid_data){
		throw Error(" **Invalid data during table creation");
	}
	else{
		columns = list_columns;
	}
}

/* FIXME: Deconstructor
*/
Relation::~Relation(){}

/* Finds item in list and returns the index
*/
int Relation::index_of(vector<string> list, string item)
{
	int index;

	for(index = list.size() - 1; index > -1; index--){
		cout << "\nAttr:" << list[index] << "item:" << item << endl;
		if (list[index] == item){
			cout << "\nFound match.";
			break;
		}
	}

	return index;
}

/* Finds item in list and returns the index
*/
int Relation::index_of(vector<int> list, int item){
	int index;

	for(index = list.size() - 1; index > -1; index--){
		if (list[index] == item){
			break;
		}
	}

	return index;
}

vector<string> Relation::get_row(int index){
	vector<string> dummy(1);
	return dummy;
}

vector<string> Relation::get_column(int index){
	vector<string> dummy(1);
	return dummy;
}

/* Add a column to the table
*/
void Relation::add_column(Column new_column){
	columns.push_back(new_column);
}

/* FIXME: Remove the given column, might take iterator
*/
void Relation::remove_column(int index){
}

/* Add a row to the table
*/
void Relation::add_tuple(vector<string> tuple){
	if (tuple.size() != columns.size()){
		throw Error(" **Row sizes must match");
	}

	for(unsigned int i = 0; i < tuple.size(); i++){
		columns[i].add_item(tuple[i]);
	}
}

/* FIXME: (arg could be iterator), remove the same index from each column.
*/
void Relation::remove_row(int index){
}

/* size():
    Return the size of the table.  Length of one of the height vectors
*/
int Relation::size(){
	return columns[0].get_length();
}

/* display():
	Prints out the relation table to the screen
*/
void Relation::display(){
	cout << "Relation: " << name << endl;

	//first display attribute names, and if they are primary keys
	for(unsigned int col = 0; col < columns.size(); col++){
		string str_output = (columns[col].isPrimary())? "*" : "";
		str_output += columns[col].get_name();

		cout << left << setw(15);
		cout << str_output;
	}
	cout << endl;

	//then display all of the data in the relation
	for(int row = 0; row < columns[0].get_length(); row++){
		for(unsigned int col = 0; col < columns.size(); col++){
			cout << left << setw(15);
			cout << columns[col].get_item(row);
		}
		cout << endl;
	}
}        

/* FIXME:
*/
void Relation::compare(string &attr, string &entry, string &op, vector<bool> &matches){
}

/* FIXME:
*/
void Relation::update_attrs(vector<string> &attr, vector<string> &literals, int index){
}
