#include <iostream>
#include <string>
#include <vector>
#include "relation.hpp"
#include "database.hpp"
#include "parser.hpp"

using namespace std;

// Make some test strings:
string test1  = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
string test2  = "INSERT INTO animals VALUES FROM (\"Joe\", \"cat\", 4);";
string test3  = "INSERT INTO animals VALUES FROM (\"Spot\", \"dog\", 10);";
string test4  = "INSERT INTO animals VALUES FROM (\"Snoopy\", \"dog\", 3);";
string test5  = "INSERT INTO animals VALUES FROM (\"Tweety\", \"bird\", 1);";
string test6  = "INSERT INTO animals VALUES FROM (\"Joe\", \"bird\", 2);";
string test7  = "dogs <- select (kind == \"dog\") animals;";
string test8  = "old_dogs <- select (age > 10) dogs;";
string test9  = "cats_or_dogs <- dogs + (select (kind == \"cat\") animals;";
string test10 = "CREATE TABLE species (kind VARCHAR(10)) PRIMARY KEY (kind);";
string test11 = "INSERT INTO species VALUES FROM RELATION project (kind) animals;";
string test12 = "a <- rename (aname, akind) (project (name, kind) animals;";
string test13 = "common_names <- project (name) (select (aname == name || akind != kind) (a * animals);";
string test14 = "answer <- common_names;";
string test15 = "UPDATE animals SET kind = \"fish\", age = 7 WHERE name == \"Joe\"";
string test16 = "DELETE FROM animals WHERE name == \"Joe\"";
string test17 = "blah blah blah + hey hey hey";
string test18 = "relation1 * relation2";
string test19 = "relation3 JOIN relation4";

string fail1 = "BAD input hahaha;";
string fail2 = "INSERTINTO animals VALUES FROM (\"Joe\", \"cat\", 4);";
string fail3 = "INSERT INTOanimals VALUES FROM (\"Joe\", \"cat\", 4);";
string fail4 = "INSERT INTO animals VALUES FROM(\"Joe\", \"cat\", 4);";

vector<string> test_str;

int main(){
	Database dbms;

	// TEST: Create Test String vector:
	test_str.push_back(test1);
	test_str.push_back(test2);
	test_str.push_back(test3);
	test_str.push_back(test4);
	test_str.push_back(test5);
	test_str.push_back(test6);
	test_str.push_back(test7);
	test_str.push_back(test8);
	test_str.push_back(test9);
	test_str.push_back(test10);
	test_str.push_back(test11);
	test_str.push_back(test12);
	test_str.push_back(test13);
	test_str.push_back(test14);
	test_str.push_back(test15);
	test_str.push_back(test16);
	test_str.push_back(test17);
	test_str.push_back(test18);
	test_str.push_back(test19);

	test_str.push_back(fail1);
	test_str.push_back(fail2);
	test_str.push_back(fail3);
	test_str.push_back(fail4);

	cout << "\n-- Database Test Suite\n";
	cout << "Enter database command or: \n";
	cout << "   <t> to run string tests\n";
	cout << "   <q> to quit\n";
	cout << "> ";
		
	string str;
    while(getline(cin, str)){
        if(str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

		if(str[0] == 't'){
			for(unsigned int i = 0; i < test_str.size(); i++){
				cout << "\n\nTesting: " << test_str[i] << "\n";
				dbms.execute(test_str[i]);
			}
		}
		cout << "\n> ";
    }
	return 0;
}

