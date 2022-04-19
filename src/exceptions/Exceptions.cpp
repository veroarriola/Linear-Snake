/*
 * Exceptions.cpp
 *
 *  Created on: 29/06/2017
 *      Author: blackzafiro
 */

#include "exceptions/Exceptions.h"

///
/// NOT SUPPORTED EXCEPTION
///

NotSupportedException::NotSupportedException(const char* msg)
{
	_msg = std::string(msg);
	_msg += " is not supported yet.";
}

const char* NotSupportedException::what() const throw()
{
	return _msg.c_str();
}

FileFormatException::FileFormatException(const char* msg)
{
	_msg = std::string(msg);
}

const char* FileFormatException::what() const throw()
{
	return _msg.c_str();
}

IllegalArgumentException::IllegalArgumentException(const char* msg)
{
	_msg = std::string(msg);
}

const char* IllegalArgumentException::what() const throw()
{
	return _msg.c_str();
}
