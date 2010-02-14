#include <iostream>
#include <string>
#include <vector>
#include "relation.hpp"
#include "database.hpp"
#include "parser.hpp"

using namespace std;

// Make some test strings:
string pass1  = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
string pass2  = "INSERT INTO animals VALUES FROM (\"Joe\", \"cat\", 4);";
string pass3  = "INSERT INTO animals VALUES FROM (\"Spot\", \"dog\", 10);";
string pass4  = "INSERT INTO animals VALUES FROM (\"Snoopy\", \"dog\", 3);";
string pass5  = "INSERT INTO animals VALUES FROM (\"Tweety\", \"bird\", 1);";
string pass6  = "INSERT INTO animals VALUES FROM (\"Joe\", \"bird\", 2);";
string pass7  = "dogs <- select (kind == \"dog\") animals;";
string pass8  = "old_dogs <- select (age > 10) dogs;";
string pass9  = "cats_or_dogs <- dogs + (select (kind == \"cat\") animals;";
string pass10 = "CREATE TABLE species (kind VARCHAR(10)) PRIMARY KEY (kind);";
string pass11 = "INSERT INTO species VALUES FROM RELATION project (kind) animals;";
string pass12 = "a <- rename (aname, akind) (project (name, kind) animals;";
string pass13 = "common_names <- project (name) (select (aname == name || akind != kind) (a * animals);";
string pass14 = "answer <- common_names;";

string fail1 = "BAD input hahaha;";
string fail2 = "INSERTINTO animals VALUES FROM (\"Joe\", \"cat\", 4);";
string fail3 = "INSERT INTOanimals VALUES FROM (\"Joe\", \"cat\", 4);";
string fail4 = "INSERT INTO animals VALUES FROM(\"Joe\", \"cat\", 4);";

vector<string> test_str;

int main(){
	Database dbms;

	// TEST: Create Test String vector:
	test_str.push_back(pass1);
	test_str.push_back(pass2);
	test_str.push_back(pass3);
	test_str.push_back(pass4);
	test_str.push_back(pass5);
	test_str.push_back(pass6);
	test_str.push_back(pass7);
	test_str.push_back(pass8);
	test_str.push_back(pass9);
	test_str.push_back(pass10);
	test_str.push_back(pass11);
	test_str.push_back(pass12);
	test_str.push_back(pass13);
	test_str.push_back(pass14);

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

