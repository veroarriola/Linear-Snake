/*
 * Exception.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_EXCEPTION_H_
#define INCLUDE_EXCEPTION_H_

#include <exception>

class IllegalStateException : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "Program reached a state it should not be in from construction";
	}
};



#endif /* INCLUDE_EXCEPTION_H_ */
