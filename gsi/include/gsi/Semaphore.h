/*******************************************************************************
 *
 * File: Semaphore.h
 * 	Generic System Interface Semaphore wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#if defined (PTHREADS)
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#elif defined(VXWORKS)
#include <semLib.h>

#elif defined(_WRS_KERNEL)
#include <semLib.h>

#elif defined(_WINDOWS)
#include <windows.h>

#else
#error "Supported platform is not defined"

#endif

namespace gsi
{

/*******************************************************************************
 *
 * This class provides a platform independent interface to semaphores.
 *
 ******************************************************************************/
class Semaphore
{
	public:
		Semaphore(unsigned int value=1, int max=1);
		~Semaphore(void);

		bool take(int timeout_msec=-1); // same as wait
		void give(void);				// same as post

		int getValue(int timeout_msec);

	private:

#if defined (PTHREADS)
		sem_t *sem;
#elif defined(VXWORKS)  // Note: pthreads can be used on VxWorks
		SEM_ID sem;
#elif defined(_WINDOWS)    // Note: with appropriate library, pthreads can be used on windows
		HANDLE sem;
#endif
};

} // namespace gsi
