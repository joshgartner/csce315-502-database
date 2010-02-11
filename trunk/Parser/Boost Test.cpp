#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

#include <iostream>
#include "ParseTest.hpp"

using namespace boost::xpressive;

// Semantic Action, pushes argument into a vector
struct pusher{
    typedef void result_type; // Result type, needed for xpressive

    template<typename Vector, typename Value>
    void operator()(Vector &vec, Value const &val) const{
        vec.push_back(val);
    }
};

function<pusher>::type const push_arg = {{}};

int main(){
	// Declare variables to hold parsed data
	std::string r_name = "";         // Name of the relation
	std::vector<std::string> data1;  // Ex: attribute names
	std::vector<std::string> data2;  // Ex: primary keys
	//std::vector<std::string> data3;  // Ex: literal values
	
	// Define commands
	sregex command, cmd_create, cmd_update, cmd_insert, cmd_delete;
	sregex attr_list, type, literals;
	sregex condition;  // FIXME - just a placeholder, involves queries.
	

	literals   = (as_xpr('"') >> *_s >> +alnum >> *_s >> '"' | +_d);
	condition  = +_w; // FIXME

	type       = (s1 = (icase("VARCHAR") >> '(' >> +_d >> ')'))[push_arg(ref(data2), s1)] | 
		         (s1 = icase("INTEGER"))[push_arg(ref(data2), s1)];

	attr_list  = '(' >> (s1 = +_w) [push_arg(ref(data1), s1)] >> 
		         +_s >> type >> *(*_s >> ',' >> *_s >> (s2 = +_w) [push_arg(ref(data1), s2)] >> +_s >> type) >> ')';

	cmd_create = (icase("CREATE TABLE") >> +_s >> (s1 = +_w)[ref(r_name) = s1] 
	             >> +_s >> attr_list >> +_s >>
		         icase("PRIMARY KEY")  >> +_s >> '(' >> *_s >> +_w >> *(*_s >> ',' >> *_s >> +_w) >>
				 ')' >> *_s >> ';');

	cmd_update = icase("UPDATE") >> +_s >> +_w >> +_s >> icase("SET") >> +_s >>
		         +_w >> *_s >> '=' >> *_s >> +_w >>
				 *(*_s >> ',' >> *_s >> +_w >> *_s >> '=' >> *_s >> +_w) >> +_s >> 
		         icase("WHERE") >> +_s >> condition >> *_s >> ';';

	cmd_insert = icase("INSERT INTO") >> +_s >> +_w >> +_s >> icase("VALUES FROM") >> +_s >>
		         '(' >> *_s >> literals >> *(*_s >> ',' >> *_s >> literals) >> ')' >> *_s >> ';';
				 
	cmd_delete = icase("DELETE FROM") >> +_s >> +_w >> +_s >> icase("WHERE") >>
		         +_s >> condition >> *_s >> ';';

	command    = +(cmd_create | cmd_update | cmd_insert | cmd_delete);

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
	// Now try and match them:
	std::string str;
    while(std::getline(std::cin, str)){
        if(str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

		if(str[0] == 't'){
			for(int i = 0; i < 1; i++){
				std::cout << "Testing: " << test_str[i] << "\n";
				if(regex_match(test_str[i], command)){
					std::cout << r_name << "\n";
					for(int j = 0; j < data1.size(); j++){
						std::cout << data1[j] << ", ";
					}
					std::cout << "\n\n";
					for(int k = 0; k < data2.size(); k++){
						std::cout << data2[k] << ", ";
					}
					std::cout << "\n";
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
		}else if(regex_match(str, command)){
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


