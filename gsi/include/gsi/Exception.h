/*******************************************************************************
 *
 * File: Exception.h
 * 	Generic System Interface class for throwing Error messages and data
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>

#if defined(PTHREADS)
#include <stdint.h>

#elif defined(VXWORKS)
#include <stdint.h>

#elif defined(_WRS_KERNEL) // WindRiver for FIRST

#elif defined(_WINDOWS)
#include <stdint.h>

#else
#error "Supported platform is not defined" 

#endif

#include <sstream>
#include <string>

#include <exception>
#include <stdexcept>

namespace gsi
{

/*******************************************************************************
 *
 * This class provides a platform independent interface to semaphores.
 *
 ******************************************************************************/
class Exception : public std::runtime_error
{
	public:
		Exception(const std::string msg, int32_t code,
			const std::string file = "", int32_t line = 0) throw();

		Exception(const std::string msg, int32_t code, Exception &cause,
			const std::string file = "", int32_t line = 0) throw();

		virtual ~Exception(void) throw();

		std::string getMessage();
		int32_t	getCode();
		std::string getFile();
		int32_t	getLine();
		std::string getStack();

		std::string toString(void);

	private:
		std::string exception_message;
		int32_t	exception_error_code;
		std::string exception_file;
		int32_t	exception_line;
		std::string exception_stack;

		void buildStackTrace(void);
		void addToStack(std::stringstream &ss, char *module,	char *addr,
			char *offset, char *name);
};

} // namespace gsi

std::ostream& operator<<(std::ostream & ost, gsi::Exception& lh);
