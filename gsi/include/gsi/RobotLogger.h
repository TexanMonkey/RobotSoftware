/****************************************************************************
 *
 * File: RobotLogger.h
 *  For detailed logging to both file and console simultaneously
 * 
 * Written by Joseph P. Foster
 *
 *
 ***************************************************************************/
#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

namespace gsi
{
class RobotLogger
{
	public:
		RobotLogger(const char* name, const char* file_name = "\0");
		~RobotLogger();
		void robotPrintf(const char* format, ...);
		static void initialize();
		static void anonymousPrintf(const char* format, ...);
		static void terminate();
	private:
		static FILE* def_log;
		FILE* log;
		char name[64];
};
}

