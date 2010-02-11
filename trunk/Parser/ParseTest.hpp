#include <string>
#include <vector>

// Make some test strings:
std::string pass1 = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
std::string pass2 = "INSERT INTO animals VALUES FROM (\"Joe\", \"cat\", 4);";
std::string pass3 = "INSERT INTO animals VALUES FROM (\"Spot\", \"dog\", 10);";
std::string pass4 = "INSERT INTO animals VALUES FROM (\"Snoopy\", \"dog\", 3);";
std::string pass5 = "INSERT INTO animals VALUES FROM (\"Tweety\", \"bird\", 1);";
std::string pass6 = "INSERT INTO animals VALUES FROM (\"Joe\", \"bird\", 2);";

std::string fail1 = "BAD input hahaha;";
std::string fail2 = "INSERTINTO animals VALUES FROM (\"Joe\", \"cat\", 4);";
std::string fail3 = "INSERT INTOanimals VALUES FROM (\"Joe\", \"cat\", 4);";
std::string fail4 = "INSERT INTO animals VALUES FROM(\"Joe\", \"cat\", 4);";


std::vector<std::string> test_str;
