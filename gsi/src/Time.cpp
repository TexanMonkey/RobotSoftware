/*******************************************************************************
 *
 * File: Time.cpp
 *	Generic System Interface Time wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Time.h"

namespace gsi
{

/*******************************************************************************
 *
 * This 
 *
 ******************************************************************************/
double Time::getTime(void)
{
#if defined (LINUX)
	struct timeval tm;
	int result = gettimeofday( &tm, NULL );
	return (double)tm.tv_sec + ((double)tm.tv_usec / 1000000.0);

#elif defined(VXWORKS)
	return System::getTime();

#elif defined(_WRS_KERNEL)
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME,&tp);
	return( (double)tp.tv_sec + (double)((double)tp.tv_nsec*1e-9));

#elif defined(_WINDOWS)
	FILETIME tm;
	GetSystemTimeAsFileTime( &tm );
	return ((double)(((uint64_t)tm.dwHighDateTime << 32) | (uint64_t)tm.dwLowDateTime)) / 10000000.0;

#endif

}

} // namespace gsi
