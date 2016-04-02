/*******************************************************************************
 *
 * File: Time.h
 *	Generic System Interface Time wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#if defined(WIN32)
#include <stdint.h>
#include <windows.h>

#elif defined(LINUX)
#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>

#elif defined(VXWORKS) 

#elif defined(_WRS_KERNEL)
#include <time.h>

#else
#pragma warning("Supported platform is not defined")

#endif


namespace gsi
{

/*******************************************************************************
 *
 * This class provides an object oriented way to interact with Time and clocks.
 *
 ******************************************************************************/
class Time
{
	public:
		static double getTime(void);
};

} // namespace gsi
