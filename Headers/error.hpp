#ifndef _error_hpp_      // include file only once
#define _error_hpp_

#include <string>

using namespace std;

class Error{
public:
	string message;
	Error(string msg){
		message = msg;
	}
};

#endif