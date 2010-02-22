#ifndef _error_hpp_      // include file only once
#define _error_hpp_

/*
File   : error.hpp
Authors: Darren White
Team   : Team X
*/

#include <string>
#include <exception>

using namespace std;

/* class Error:
	A class for throwing errors.  Extends the std::exception class
*/
class Error : public exception{
public:
	string message;
	virtual const char* what() const throw(){
		if(message.empty()){
			return " ** Database Error **";
		}
		else
			return message.c_str();
	}

	Error(string msg){
		message = msg;
	}
};

#endif