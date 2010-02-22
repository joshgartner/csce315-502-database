#include <iostream>
#include <string>
#include <vector>
#include "relation.hpp"
#include "database.hpp"
#include "parser.hpp"

using namespace std;

// Make some test strings:
string create1 = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
string create2 = "CREATE TABLE cars (make VARCHAR(20), model VARCHAR(20), year INTEGER) PRIMARY KEY (make, model, year);";
string create3 = "CREATE TABLE toys (name VARCHAR(20)) PRIMARY KEY (name);";
string create4 = "CREATE TABLE big_animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
string create5 = "CREATE TABLE small_animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";

string insert1 = "INSERT INTO animals VALUES FROM (\"Joe\", \"cat\", 4);";
string insert2 = "INSERT INTO animals VALUES FROM (\"Spot\", \"dog\", 10);";
string insert3 = "INSERT INTO animals VALUES FROM (\"Snoopy\", \"dog\", 3);";
string insert4 = "INSERT INTO animals VALUES FROM (\"Tweety\", \"bird\", 1);";
string insert5 = "INSERT INTO animals VALUES FROM (\"Joe\", \"bird\", 2);";
string insert6 = "INSERT INTO cars VALUES FROM (\"VW\", \"Jetta\", 2008);";
string insert7 = "INSERT INTO cars VALUES FROM (\"Ford\", \"Ranger\", 2004);";
string insert8 = "INSERT INTO cars VALUES FROM (\"Ford\", \"Ranger\", 2000);";
string insert9 = "INSERT INTO toys VALUES FROM (\"Legos\");";
string insert10 = "INSERT INTO cars VALUES FROM RELATION animals;";
string insert11 = "INSERT INTO big_animals VALUES FROM (\"Bob\", \"bear\", 3);";
string insert12 = "INSERT INTO big_animals VALUES FROM (\"Harry\", \"elephant\", 5);";
string insert13 = "INSERT INTO small_animals VALUES FROM (\"Spot\", \"dog\", 10);";
string insert14 = "INSERT INTO small_animals VALUES FROM (\"Tweety\", \"bird\", 1);";

string rename1 = "a <- rename (aname, akind, ayears) animals;";
string rename2 = "a <- rename (aname, akind) animals;";

string select1 = "dogs <- select (kind == \"dog\") animals;";
string select2 = "dogs <- select (kind == \"dog\" && (name == \"Spot\")) animals;";
string select3 = "new_and_old <- select (model == \"Jetta\" || (year < 2001)) cars;";
string select4 = "best_toys <- select (name == \"GI Joe\") toys;";

string project1 = "animal_names <- project (name) animals;";
string project2 = "car_types <- project (model, year) cars;";

string update1 = "UPDATE animals SET kind = \"fish\", years = 7 WHERE name == \"Joe\";";
string update2 = "UPDATE animals SET kind = \"fish\", age = 7 WHERE name == \"Joe\";";
string update3 = "UPDATE animals SET kind = \"fish\", age = "" WHERE name == \"Joe\";";
string update4 = "UPDATE cars SET make = \"CC\", year = 2007 WHERE make == \"VW\";";
string update5 = "UPDATE toys SET name = \"GIJoe\" WHERE name == \"Legos\";";

string delete1 = "DELETE FROM animals WHERE name == \"Joe\";";
string delete2 = "DELETE FROM cars WHERE year < \"2008\";";

string union1 = "all_animals <- big_animals + animals;";

string product1 = "x_animals <- big_animals * animals;";

string difference1 = "diff_animals <- animals - small_animals;";

string test1 = "c <- a + b;";
string test2 = "c <- a - b;";
string test3 = "c <- a * b;";
string test4 = "c <- a JOIN b;";
string test5 = "a + b";
string test6 = "a - b";
string test7 = "a * b";
string test8 = "a JOIN b";
string test9 = "c <- (a - b) + (select (kind == \"dog\") animals);";
string test10 = "c <- (select (kind == \"dog\") animals) + (a - b);";


vector<string> test_str;

int main(){
	Database dbms;

	// TEST: Create Test String vector:
	test_str.push_back(create1); // Create
	test_str.push_back(create2);
	test_str.push_back(create3);
	test_str.push_back(create4);
	test_str.push_back(create5);
	test_str.push_back(insert1); // Insert
	test_str.push_back(insert2);
	test_str.push_back(insert3);
	test_str.push_back(insert4);
	test_str.push_back(insert5);
	test_str.push_back(insert6);
	test_str.push_back(insert7);
	test_str.push_back(insert8);
	test_str.push_back(insert10);
	test_str.push_back(insert11);
	test_str.push_back(insert12);
	test_str.push_back(insert13);
	test_str.push_back(insert14);
	test_str.push_back(rename1);
	test_str.push_back(rename2);
	test_str.push_back(project1); // Project
	test_str.push_back(project2);
	test_str.push_back(union1);
	test_str.push_back(product1);
	test_str.push_back(difference1);
	/*
	test_str.push_back(select1); // Select
	test_str.push_back(select2);
	test_str.push_back(select3);
	test_str.push_back(select4);
	test_str.push_back(update1); // Update
	test_str.push_back(update2); 
	test_str.push_back(update3);
	test_str.push_back(update4);
	test_str.push_back(update5);
	test_str.push_back(delete1); // Delete
	test_str.push_back(delete2);

	test_str.push_back(test1);
	test_str.push_back(test2);
	test_str.push_back(test3);
	test_str.push_back(test4);
	test_str.push_back(test5);
	test_str.push_back(test6);
	test_str.push_back(test7);
	test_str.push_back(test8); 
	test_str.push_back(test9);
	test_str.push_back(test10); */

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
			for(int i = 0; i < (int) test_str.size(); i++){
				cout << "\n\nTesting: " << test_str[i] << "\n";
				dbms.execute(test_str[i]);
			}
		}
		cout << "\n> ";
    }
	return 0;
}

