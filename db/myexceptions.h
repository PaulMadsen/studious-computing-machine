#ifndef MYEXCEPTIONS_H
#define MYEXCEPTIONS_H
#include <exception>

class myExceptions : public std::exception {
	virtual const char* what() const 
	{
		return "An unspecified exception occured\n";
	}
};
class notImplementedException : public myExceptions {
	const char* what() const
	{
		return "A function was called that was not yet implemented\n";
	}
};

#endif