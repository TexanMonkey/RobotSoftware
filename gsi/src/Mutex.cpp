/*******************************************************************************
 *
 * File: Mutex.cpp
 * 	Generic System Interface class for mutexes
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Mutex.h"
#include <stdio.h>

//#include <unistd.h>
//#include <sys/syscall.h>
//#include <sys/types.h>

namespace gsi
{

/*******************************************************************************
 *
 ******************************************************************************/
Mutex::Mutex(void)
{
#if defined (PTHREADS)
	pthread_mutexattr_t mtex_attr;

	pthread_mutexattr_init(&mtex_attr);
    pthread_mutexattr_settype(&mtex_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_settype(&mtex_attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&mtex, &mtex_attr);

	pthread_mutexattr_destroy(&mtex_attr);

#elif defined(VXWORKS)
	mtex = semMCreate(SEM_Q_PRIORITY|SEM_DELETE_SAFE|SEM_INVERSION_SAFE, SEM_FULL);

#elif defined(_WINDOWS)
	mtex = CreateMutex(NULL, false, NULL);

#endif
}

/*******************************************************************************
 *
 ******************************************************************************/
Mutex::~Mutex(void)
{
#if defined (PTHREADS)
	pthread_mutex_destroy(&mtex);

#elif defined(VXWORKS)
	semDestroy(mtex);

#elif defined(_WINDOWS)
	CloseHandle(mtex);

#endif
}

/*******************************************************************************
 *
 ******************************************************************************/
void Mutex::lock(void)
{
#if defined (PTHREADS)
//	printf("Mutex::lock request : for %08X  by %08X  data %08X %08X  \n",
//			(uint32_t)&mtex, (uint32_t)syscall(SYS_gettid), mtex.__data.__count, mtex.__data.__owner);
	pthread_mutex_lock(&mtex);
//	printf("Mutex::lock obtained: for %08X  by %08X  data %08X %08X  \n",
//			(uint32_t)&mtex, (uint32_t)syscall(SYS_gettid), mtex.__data.__count, mtex.__data.__owner);
#elif defined(VXWORKS)
	semTake(mtex, WAIT_FOREVER);

#elif defined(_WINDOWS)
	WaitForSingleObject(mtex, INFINITE);

#endif
}

/*******************************************************************************
 *
 ******************************************************************************/
void Mutex::unlock(void)
{
#if defined (PTHREADS)
	pthread_mutex_unlock(&mtex);
//	printf("Mutex::lock released: for %08X  by %08X  data %08X %08X  \n",
//			(uint32_t)&mtex, (uint32_t)syscall(SYS_gettid), mtex.__data.__count, mtex.__data.__owner);

#elif defined(VXWORKS)
	semGive(mtex);

#elif defined(_WINDOWS)
	ReleaseMutex(mtex);

#endif
}

/*******************************************************************************
 *
 ******************************************************************************/
bool Mutex::tryLock(double seconds)
{
#if defined (PTHREADS)
    if (seconds == 0.0)
    {
    	// try to lock, but don't wait for it
		return (int)((pthread_mutex_trylock(&mtex) == 0));
	}
	else
	{
		// try to lock, but only wait for so much time
		struct timespec dtime;
		dtime.tv_sec = seconds;
		dtime.tv_nsec = (seconds - dtime.tv_sec) * 1000000;

		return (int)((pthread_mutex_timedlock(&mtex, &dtime) == 0));
	}

#elif defined(VXWORKS)
	return (semTake(mtex, (uint32_t)(seconds * 1000000)) == OK);

#elif defined(_WINDOWS)
	return (WaitForSingleObject(mtex, (uint32_t)(seconds * 1000000)) == WAIT_OBJECT_0);

#else
	return false;
#endif
}

} // namespace gsi
