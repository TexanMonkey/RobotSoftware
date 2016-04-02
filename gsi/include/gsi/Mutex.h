/*******************************************************************************
 *
 * File: Mutex.h
 * 	Generic System Interface Mutex wrapper, also includes a simple
 * 	MutexScopeLock class.
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
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#elif defined(VXWORKS)
#include <stdint.h>
#include <semLib.h>

#elif defined(_WRS_KERNEL) // WindRiver for FIRST
#include <semLib.h>

#elif defined(_WINDOWS)
#include <stdint.h>
#include <windows.h>

#else
#error "Supported platform is not defined" 

#endif

namespace gsi
{

/*******************************************************************************
 *
 * This class provides a platform independent interface to a mutex.
 *
 ******************************************************************************/
class Mutex
{
	public:
		Mutex(void);
		virtual ~Mutex(void);

		virtual void lock(void); 
		virtual void unlock(void);					 
		virtual bool tryLock(double seconds = 0.0);

	private:
#if defined (PTHREADS)
		pthread_mutex_t mtex;

#elif defined(VXWORKS) || defined(_WRS_KERNEL)
		SEM_ID mtex;

#elif defined(_WINDOWS)
		HANDLE mtex;

#endif
};

/*******************************************************************************
 *
 * Create an instance of this class to lock the provided Mutex until the
 * instance of this class looses scope.
 *
 ******************************************************************************/
class MutexScopeLock
{

	public:
		MutexScopeLock(Mutex &m) : mtex(m) { mtex.lock(); }
		~MutexScopeLock(void)    { mtex.unlock(); }

	private:
		Mutex &mtex;
};

} // namespace gsi
