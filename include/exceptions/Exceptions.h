/*
 * Exceptions.h
 *
 *  Created on: 29/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_EXCEPTIONS_EXCEPTIONS_H_
#define INCLUDE_EXCEPTIONS_EXCEPTIONS_H_

#include <iostream>
#include <exception>
#include <string>

class NotSupportedException : public std::exception
{
private:
	std::string _msg;
public:
	NotSupportedException(const char* msg);
	const char* what() const throw();
};

class FileFormatException : public std::exception
{
private:
	std::string _msg;
public:
	FileFormatException(const char* msg);
	const char* what() const throw();
};

class IllegalArgumentException : public std::exception
{
private:
	std::string _msg;
public:
	IllegalArgumentException(const char* msg);
	const char* what() const throw();
};


#endif /* INCLUDE_EXCEPTIONS_EXCEPTIONS_H_ */
