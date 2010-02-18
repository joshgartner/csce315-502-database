#ifndef _error_hpp_      // include file only once
#define _error_hpp_

#include <string>
#include <exception>

using namespace std;

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