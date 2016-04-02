/****************************************************************************
 *
 * File: RobotLogger.h
 *  For detailed logging to both file and console simultaneously
 * 
 * Written by Joseph P. Foster
 *
 *
 ***************************************************************************/

#include "gsi/RobotLogger.h"

namespace gsi
{
	FILE* RobotLogger::def_log = NULL;
	void RobotLogger::initialize()
	{
		def_log = fopen("RobotLog.txt", "a");
	}
	
	
	void RobotLogger::anonymousPrintf(const char* format, ...)
	{
		va_list ap;
		va_start(ap,format);
		vprintf(format,ap);
		va_end(ap);
		va_start(ap,format);
		vfprintf(def_log,format,ap);
		va_end(ap);
	}
	
	void RobotLogger::terminate()
	{
		if(def_log == NULL)
		{
			fclose(def_log);
			def_log = NULL;
		}
	}
	
	RobotLogger::RobotLogger(const char* tag_name, const char* file_name)
	{
		if(strcmp(file_name,"\0") == 0)
		{
			log = def_log;
		}
		else
		{
			log = fopen(file_name, "a");
		}
		strcpy(name,tag_name);
	}
	
	void RobotLogger::robotPrintf(const char* format, ...)
	{
		char fmt[512];
		sprintf(fmt,"%-18s:%s",name,format);
		/*
		va_list ap;
		va_start(ap,format);
		vprintf(fmt,ap);
		va_end(ap);
		va_start(ap,format);
		vfprintf(log,fmt,ap);
		va_end(ap);
		fflush(log);
		*/
		
	}
	
	RobotLogger::~RobotLogger()
	{
		if(def_log == NULL)
		{
			fclose(log);
			log = NULL;
		}
	}
	
	
}

