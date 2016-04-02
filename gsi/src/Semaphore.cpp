/*******************************************************************************
 *
 * File: Semaphore.cpp
 * 	Generic System Interface Semaphore wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Semaphore.h"

#include "gsi/Exception.h"

#include <stdio.h>
#include <string.h>

#if defined(LINUX)
#include <time.h>
#endif

namespace gsi
{

/*******************************************************************************
 *
 ******************************************************************************/
Semaphore::Semaphore(unsigned int value, int max)
{
#if defined (PTHREADS)
	sem = (sem_t *)malloc(sizeof(sem_t));
	if (sem == NULL)
	{
		throw Exception("Could not allocate semaphore handle", errno, __FILE__, __LINE__);
	}

	if (sem_init(sem, 0, value) != 0)
	{
		sem = NULL;
		throw Exception("Could not create Semaphore", errno, __FILE__, __LINE__);
	}

#elif defined(VXWORKS)
	sem = semCCreate(SEM_Q_FIFO,value);
	if (sem == NULL)
	{
		throw Exception("Could not create Semaphore", -1, __FILE__, __LINE__);
	}

#elif defined(_WINDOWS)
	sem = NULL;
	SECURITY_ATTRIBUTES attr;

	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle = TRUE;

	sem = CreateSemaphore(&attr,value,max,NULL);
	if (sem == NULL)
	{
		throw Exception("Could not create Semaphore", -1, __FILE__, __LINE__);
	}

#endif

}

/*******************************************************************************
 *
 ******************************************************************************/
Semaphore::~Semaphore(void)
{
#if defined (PTHREADS)
	if (sem != NULL)
	{
		if (sem_destroy(sem) != 0)
		{
			throw Exception("Could not destroy Semaphore", errno, __FILE__, __LINE__);
		}

		free(sem);
		sem = NULL;
	}

#elif defined(VXWORKS)
	if (sem != NULL)
	{
		semDelete(sem);
		sem = NULL;
	}

#elif  defined(_WINDOWS)
	if (sem != NULL)
	{
		CloseHandle(sem);
		sem = NULL;
	}

#endif
}

/*******************************************************************************
 *
 * @param	msec	the number of msec this should wait for the semaphore,
 * 					if negative, wait forever if needed, default value = -1
 *
 * @return	true if the semaphore is now in this threads control, false if
 * 			msec has passed without getting control.
 *
 * @throws Error if anything fails
 *
 ******************************************************************************/
bool Semaphore::take(int timeout_msec)
{
	bool ret_val = false;
#if defined (PTHREADS)

	if (timeout_msec > 0)
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);

		ts.tv_sec  += (timeout_msec / 1000);
		ts.tv_nsec += (timeout_msec % 1000) * 1000000;

		while (ts.tv_nsec >= 1000000000)
		{
			ts.tv_sec += 1;
			ts.tv_nsec -= 1000000000;
		}

		if (sem_timedwait(sem, &ts) != 0)
		{
			if (errno == ETIMEDOUT)
			{
				return false;
			}
			else
			{
				throw Exception("Error waiting for semaphore", errno, __FILE__, __LINE__);
			}
		}
	}
	else
	{
		if (sem_wait(sem) != 0)
		{
			throw Exception("Error waiting for semaphore", errno, __FILE__, __LINE__);
		}
	}

	ret_val = true;

#elif defined(VXWORKS)
	int ret = semTake(sem,timeout_msec);
	if (ret == ERROR && errno == S_objLib_OBJ_TIMEOUT)
	{
		return false;
	}
	else if (ret != OK)
	{
		ret_val = false;
	}
	else 
	{
		ret_val = true;
	}

#elif defined(_WINDOWS)
	if (WaitForSingleObject(sem,timeout_msec) == WAIT_OBJECT_0)
	{
		ret_val =  true;
	}
	else
	{
		ret_val = false;
	}
#endif

	return ret_val;
}

/*******************************************************************************
 *
 ******************************************************************************/
void Semaphore::give()
{
#if defined (PTHREADS)
	if (sem_post(sem))
	{
		throw Exception("Could not release semaphore", errno, __FILE__, __LINE__);
	}

#elif defined(VXWORKS)
	if (semGive(sem))
	{
		throw Exception("Could not release semaphore", -1, __FILE__, __LINE__);
	}

#elif defined(_WINDOWS)
	if (! ReleaseSemaphore(sem,1,NULL))
	{
		throw Exception("Could not release semaphore", GetLastError(), __FILE__, __LINE__);
	}

#endif
}


/*******************************************************************************
 *
 ******************************************************************************/
int Semaphore::getValue(int timeout_msec)
{
	int value = timeout_msec;  // just to stop compiler warning
	value = -1;  

#if defined (PTHREADS)
	if (sem_getvalue(sem, &value) != 0)
	{
		throw Exception("Could not retrieve semaphore value", errno, __FILE__, __LINE__);
	}

#elif defined(VXWORKS)
	throw Exception("Could not retrieve semaphore value", errno, __FILE__, __LINE__);

#elif defined(_WINDOWS)
	throw Exception("Could not retrieve semaphore value", errno, __FILE__, __LINE__);

#endif

	return(value);
}

} // namespace gsi
