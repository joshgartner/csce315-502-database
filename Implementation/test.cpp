#include <iostream>
#include <string>
#include <vector>
#include "relation.hpp"
#include "database.hpp"
#include "parser.hpp"

using namespace std;

// Make some test strings:
string create1 = "CREATE TABLE table1 (name VARCHAR(20), age VARCHAR(20), letter VARCHAR(20)) PRIMARY KEY (letter);";
string create2 = "CREATE TABLE table2 (age VARCHAR(20), name VARCHAR(20), color VARCHAR(20)) PRIMARY KEY (color);";

string insert1 = "INSERT INTO table1 VALUES FROM (\"Joe\", 12, \"A\");";
string insert2 = "INSERT INTO table1 VALUES FROM (\"Sally\", 4, \"B\");";
string insert3 = "INSERT INTO table1 VALUES FROM (\"Sally\", 13, \"C\");";
string insert4 = "INSERT INTO table1 VALUES FROM (\"Bob\", 4, \"D\");";
string insert5 = "INSERT INTO table1 VALUES FROM (\"Bill\", 44, \"E\");";
string insert6 = "INSERT INTO table1 VALUES FROM (\"Rob\", 21, \"F\");";

string insert7 = "INSERT INTO table2 VALUES FROM (13, \"Sally\", \"Red\");";
string insert8 = "INSERT INTO table2 VALUES FROM (44, \"Man\", \"Purple\");";
string insert9 = "INSERT INTO table2 VALUES FROM (12, \"Joe\", \"Blue\");";
string insert10 = "INSERT INTO table2 VALUES FROM (7, \"Bob\", \"Fushia\");";
string insert11 = "INSERT INTO table2 VALUES FROM (12, \"Joe\", \"Green\");";

string join1 = "table3 <- table1 JOIN table2;";

string complex = "test <- select (name == \"Sally\" && age < 20) (table1 JOIN table2);";

int main(){
	Database dbms;
	vector<string> test_str;
	// TEST: Create Test String vector:
	test_str.push_back(create1);
	test_str.push_back(create2);
	test_str.push_back(insert1);
	test_str.push_back(insert2);
	test_str.push_back(insert3);
	test_str.push_back(insert4);
	test_str.push_back(insert5);
	test_str.push_back(insert6); 
	test_str.push_back(insert7);
	test_str.push_back(insert8);
	test_str.push_back(insert9);
	test_str.push_back(insert10);
	test_str.push_back(insert11);
	test_str.push_back(join1);
	test_str.push_back(complex);

	cout << "\n-- Database Test Suite\n";
	cout << "Enter database command or: \n";
	cout << "   <t> to run string tests\n";
	cout << "   <q> to quit\n";
	cout << "> ";
	
	string table1 = "table1";
	string table2 = "table2";

	dbms.load(table1);
	dbms.load(table2);

	string str;
    while(getline(cin, str)){
        if(str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

		try{
			if(str[0] == 't'){
				for(int i = 0; i < (int) test_str.size(); i++){
					cout << "\n\nTesting: " << test_str[i] << "\n";
					dbms.execute(test_str[i]);
				}
			}
		}catch(exception& e){
			cout << e.what();
		}
		cout << "\n> ";
    }
	return 0;
}

