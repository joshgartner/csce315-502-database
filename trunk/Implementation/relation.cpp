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

	if(attributes.size() != types.size()){
		cout << "\nNumber of attributes must match types";
		b_valid_data = false;
	}

	//if we got data, we must have at least one primary key
	if((attributes.size() > 0) && (primary_keys.size() == 0)){
		cout << "\nMust have at least one attribute and primary key";
		b_valid_data = false;
	}

	//make sure each primarykey is in attributes
	for(int i = 0; i < (int) primary_keys.size(); i++){
		int index = index_of(attributes, primary_keys[i]);

		//if primary key is in attributes, remember index so we can store that in column data
		if(index != -1){
			primary_key_indices.push_back(index);
		}
		//if it is not, throw an error
		else{
			cout << "\nPrimary key must be an attribute";
			b_valid_data = false;
			break;
		}
	}

	vector<Column> list_columns;

	//make column from each attribute and add to columns
	for(int i = 0; i < (int) attributes.size(); i++){
		string type_id = types[i].substr(0, 7);

		//decide if this attribute is a primary key
		bool b_is_primary_key = (index_of(primary_key_indices, i) == -1);

		Column new_column(attributes[i], type_id, b_is_primary_key);
		list_columns.push_back(new_column);
	}

	if(!b_valid_data){
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
int Relation::index_of(vector<string> list, string item){
	int index;

	for(index = 0; index < (int) list.size(); index++){
		if(list[index].compare(item) == 0){
			return index;
		}
	}
	return -1;
}

/* Finds item in list and returns the index
*/
int Relation::index_of(vector<int> list, int item){
	int index;

	for(index = 0; index < (int) list.size(); index++){
		if(list[index] == item){
			return index;
		}
	}
	return -1;
}

vector<string> Relation::get_tuple(int index){
	vector<string> tuple;
	vector<string> col;
	for(int i = 0; i < (int) columns.size(); i++){
		col = columns[i].get_items();
		tuple.push_back(col[index]);
	}
	return tuple;
}

vector<string> Relation::get_column(int index){
	return columns[index].get_items();
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
	if(tuple.size() != columns.size()){
		cout << "Column Size: " << columns.size();
		cout << "Tuples Size: " << tuple.size();
		//throw Error("\n **Row sizes must match");
	}

	for(unsigned int i = 0; i < tuple.size(); i++){
		columns[i].add_item(tuple[i]);
	}
}

/* FIXME: (arg could be iterator), remove the same index from each column.
*/
void Relation::remove_row(int index){
}

/* copy_attrs(Relation *r)
	Takes all the columns from this relation and copies them to r's.
	The columns are then cleared in the new relation.  Useful for queries.
*/
void Relation::copy_attrs(Relation *r){
	r->columns = columns;
	for(int i = 0; i < (int) columns.size(); i++){
		(r->columns[i]).remove_items();
	}
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
	cout << "\n\nRelation: " << name << endl;

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

/* str_compare(string left, string right):
	This function is meant to compare strings that are actually
	integers.
	Returns: 0 if equal, 1 if less than, -1 if greater than.
*/
int Relation::str_compare(string left, string right){
	int i, j;
	istringstream lss(left);
	istringstream rss(right);
	lss >> i;
	rss >> j;
	if(i == j){
		return 0;
	}
	else if(i < j){
		return 1;
	}
	else{   // i > j
		return -1;
	}
}
/* FIXME:
*/
vector<bool> Relation::compare(string &attr, string &value, string &op){
	cout << "\nComparing " << attr << " " << op << " " << value;
	// DEBUG cout << "\nWhole relation:"; this->display();
	
	vector<bool> indexes(this->size(), false);
	vector<string> data;
	for(int i = 0; i < (int) columns.size(); i++){ // Find the right column
		if(columns[i].get_name() == attr){
			cout << "\nFound attr: " << attr;
			data = columns[i].get_items();
			break;
		}
	}
	// Perform the appropriate comparison, and update match vector
	if(op == "=="){
		for(int i = 0; i < (int) data.size(); i++){
			cout << "\nDoes " << data[i] << "==" << value << "?";
			if(data[i] == value){
				indexes[i] = true;
				// DEBUG cout << "\nData matched: " << data[i] << " and " << value;
			}
		}
	}
	else if(op == "!="){
		for(int i = 0; i < (int) data.size(); i++){
			if(data[i] != value){
				indexes[i] = true;
			}
		}
	}
	else if(op == "<"){
		for(int i = 0; i < (int) data.size(); i++){
		cout << "\nIs " << data[i] << "<" << value << "?";
			if(str_compare(data[i], value) == 1){
				indexes[i] = true;
				// DEBUG cout << "\nData matched: " << data[i] << " and " << value;
			}
		}
	}
	else if(op == ">"){
		for(int i = 0; i < (int) data.size(); i++){
			if(str_compare(data[i], value) == -1){
				indexes[i] = true;
			}
		}
	}
	else if(op == "<="){
		for(int i = 0; i < (int) data.size(); i++){
			if(str_compare(data[i], value) >= 0){
				indexes[i] = true;
				// DEBUG cout << "\nData matched: " << data[i] << " and " << value;
			}
		}
	}
	else if(op == ">="){
		for(int i = 0; i < (int) data.size(); i++){
			if(str_compare(data[i], value) <= 0){
				indexes[i] = true;
			}
		}
	}
	return indexes;
}

/* FIXME:
*/
void Relation::update_attrs(vector<string> &attr, vector<string> &literals, int index){
}
