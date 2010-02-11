#include "parser.hpp"

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

int main(){
	Parser p;

	// TEST: Create Test String vector:
	test_str.push_back(pass1);
	test_str.push_back(pass2);
	test_str.push_back(pass3);
	test_str.push_back(pass4);
	test_str.push_back(pass5);
	test_str.push_back(pass6);
	test_str.push_back(fail1);
	test_str.push_back(fail2);
	test_str.push_back(fail3);
	test_str.push_back(fail4);

	std::string str;
    while(std::getline(std::cin, str)){
        if(str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

		if(str[0] == 't'){
			for(int i = 0; i < 1; i++){
				std::cout << "Testing: " << test_str[i] << "\n";
				if(p.match(test_str[i])){
					std::cout << "-------------------------\n";
					std::cout << "Parsing succeeded\n";
					std::cout << "-------------------------\n";
				}
				else{
					std::cout << "-------------------------\n";
					std::cout << "Parsing failed\n";
					std::cout << "-------------------------\n";
				}
			}
		}else if(p.match(str)){
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "-------------------------\n";
        }
        else{
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }
	return 0;
}

