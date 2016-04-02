/*******************************************************************************
 *
 * File: Thread.cpp
 * 	Generic System Interface class for threads
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Thread.h"
#include <stdio.h>

#include <sstream>

namespace gsi
{

/*******************************************************************************
 *
 * This constructor is used to create a class that will run a new thread with
 * the specified options.
 *
 * @param	name		The name of the thread.
 *
 * @param	priority	Ohe priority of this thread can be specified, 0 is 
 *						highest priority, 255 is lowest priority.  This 
 *						defaults to 100 if not specified.
 *
 * @param	options		Options for the creation of this thread.  If not
 *						specified this defaults to VX_FP_TASK.
 *
 * @param	stack_size	The size of the stack for this thread in bytes. If not
 *						specified this defaults to 32768.
 *
 ******************************************************************************/
Thread::Thread(std::string name, Thread::ThreadPriority priority, 
	uint32_t options, uint32_t stack_size)
{
	thread_name = name;
	thread_priority = priority;
	thread_options = options;
	thread_size = stack_size & 0xFFFFFFFE; // make sure it's even
	
	is_running = false;
	is_stop_requested = false;
	
    thread_id = THREAD_ID_ERROR;

#if defined(_WINDOWS)
	thread_handle = NULL;
#endif
}

/*******************************************************************************
 *
 * To be safe this destructor will simply put in a request for the thread to
 * stop.  When the run() method returns, the runHandlerImpl() method takes 
 * care of releasing the resources allocated by this class and calls finalize() 
 * to allow subclasses to release resources they allocate in a thread-safe 
 * manner.
 *
 ******************************************************************************/
Thread::~Thread(void)
{
	if (is_running)
	{
		requestStop();
	}
}

/*******************************************************************************
 *
 * If the thread is not already running, start it.  Calling start() will clear
 * any previous stop requests.
 *
 ******************************************************************************/
void Thread::start(void)
{
	if (!is_running)
	{
		is_stop_requested = false;
#if defined (PTHREADS)
		pthread_create((pthread_t *)(&thread_id),NULL,Thread::runHandle,this);

#elif defined(VXWORKS)
		thread_id = (int64_t)(taskSpawn((char *)(thread_name.c_str()), thread_priority, 
			thread_options, thread_size, 
			(FUNCPTR)&Thread::runHandle, (uint32_t)this, 0,0,0,0,0,0,0,0,0));  

		taskPrioritySet(thread_id, thread_priority);

#elif defined(_WINDOWS)
		// NULL is for default security attributes
		thread_handle = CreateThread(NULL, thread_size, Thread::runHandle,      
            this, thread_options, (LPDWORD)(&thread_id));

		SetThreadPriority(thread_handle, thread_priority);
#endif

	}
}

/*******************************************************************************
 *
 * This method will only return after the specified number of seconds 
 * (to the nearest tick) have passed.  The exact ammount of time it takes
 * for this method to return will be impacted by the systems clock resolution.
 *
 * @param	time the amount of time in seconds and fractions of a second that
 *			the calling thread should be delayed.
 *
 ******************************************************************************/
void Thread::sleep(double time)
{
	if (time > 0.0)
	{
#if defined (PTHREADS)
		struct timespec req;
		req.tv_sec = (time_t)time;
		req.tv_nsec = (long)((time - req.tv_sec) * 1000000000);
		nanosleep(&req, NULL);

#elif defined(VXWORKS)
		taskDelay((int)(System::TICK_PER_SECOND * time));

#elif defined(_WINDOWS)
		Sleep((DWORD)(time * 1000));

#endif
	}
}

/*******************************************************************************
 *
 * Set the priority that this thread should have when it is run.
 *
 * NOTICE:	setting thread priorities does not always work, it is platform
 *			user, and configuration dependant
 *
 * @param	priority	the new priority for this thread
 *
 ******************************************************************************/
void Thread::setPriority(Thread::ThreadPriority priority)
{
	thread_priority = priority;

#if defined (PTHREADS)
	// @TODO:

#elif defined(VXWORKS)
	if (is_running)
	{
		taskPrioritySet(thread_id, thread_priority);
	}

#elif defined(_WINDOWS)
	if (is_running)
	{
		SetThreadPriority(thread_handle, thread_priority);
	}
#endif
}

/*******************************************************************************
 *
 * NOTICE:	setting thread priorities does not always work, it is platform
 *			user, and configuration dependant
 *
 * @return the priority of this thread
 *
 ******************************************************************************/
Thread::ThreadPriority Thread::getPriority(void)
{
#if defined (PTHREADS)
	// @TODO:

#elif defined(VXWORKS)
	if (is_running)
	{
		taskPriorityGet(thread_id, &thread_priority);
	}

#elif defined(_WINDOWS)
	if (thread_handle != NULL)
	{
		thread_priority = (ThreadPriority)(GetThreadPriority(thread_handle));
	}
#endif

	return thread_priority;
}

/*******************************************************************************
 *
 * Set the size of the stack that should be created when this thread is run.
 * This should be an even number of bytes, if the specified value is not even,
 * it will be reduced by 1 to make it even.
 *
 * NOTE: This does not change the size of the stack of a running thread, it
 *		is the value that will be used when the thread starts.
 *
 * @param	size	the size of the stack in bytes that should be create for 
 *					this thread when run.
 *
 ******************************************************************************/
void Thread::setStackSize(uint32_t size)
{
	thread_size = size & 0xFFFFFFFE; // make sure it's even
}

/*******************************************************************************
 *
 * @returns	the number of bytes that will be allocated for this threads
 *			stack when the thread is run.
 *
 ******************************************************************************/
uint32_t Thread::getStackSize(void)
{
	return thread_size;
}

/*******************************************************************************
 *
 * Set the thread options for this thread.
 *
 * NOTE: This does not change the options for the thread if it's running, it 
 * 		only changes the value that will be used when the thread starts.
 *
 * @param	options	the new options for this thread
 *
 ******************************************************************************/
void Thread::setOptions(uint32_t options)
{
	thread_options = options;
}

/*******************************************************************************
 *
 * Add (bitwise or) the specified options into the the existing options for
 * this thread.
 *
 * NOTE: This does not change the options for the thread if it's running, it 
 * 		only changes the value that will be used when the thread starts.
 *
 * @param	options	the new options for this thread
 *
 ******************************************************************************/
void Thread::addOptions(uint32_t options)
{
	thread_options |= options;
}

/*******************************************************************************
 *
 * @returns	the options that will be used when the thread starts
 *
 ******************************************************************************/
uint32_t Thread::getOptions(void)
{
	return thread_options;
}

/*******************************************************************************
 *
 * @return true if there is currently a thread running for this class
 *
 ******************************************************************************/
bool Thread::isRunning(void)
{
	return is_running;
}

/*******************************************************************************
 *
 * This sets a flag that indicates the thread should stop the next time it
 * can safely be stopped.  It is up to the subclasses to honor this request
 * by using the isStopRequested() method periodically.
 *
 ******************************************************************************/
void Thread::requestStop(void)
{
	is_stop_requested = true;
}

/*******************************************************************************
 *
 * @return true if stop has been called since the last time start has been
 *			called.
 ******************************************************************************/
bool Thread::isStopRequested(void)
{
	return is_stop_requested;
}

/*******************************************************************************
 *
 * @return  The ID of the thread running for this class, THREAD_ID_ERROR if there is not
 * 			currently a thread running.
 *
 ******************************************************************************/
int64_t Thread::getId()
{
	return thread_id;
}

/*******************************************************************************
 *
 * @return the name of this thread.
 *
 ******************************************************************************/
std::string Thread::getName(void)
{
	return thread_name;
}

#if defined (PTHREADS)
/*******************************************************************************
 *
 * This static method matches the function prototype required by PThreads.
 * 
 * It casts the first argument to a Thread pointer and calls the
 * runHandleImpl() method of that object.
 *
 * @return this method always returns 0, the return type is required by
 *     the PThreads function prototype
 *
 ******************************************************************************/
void * Thread::runHandle(void *obj)
{
	return (void *) ((Thread *)obj)->runHandleImpl();
}

#elif defined(VXWORKS)
/*******************************************************************************
 *
 * This static method matches the function prototype required by VxWorks.
 * 
 * It casts the first argument to a Thread pointer and calls the
 * runHandleImpl() method of that object.
 *
 * @return this method always returns 0, the return type is required by
 *     the VxWorks function prototype
 *
 ******************************************************************************/
int Thread::runHandle(uint32_t obj, ...)
{
	return ((Thread *)obj)->runHandleImpl();
}

#elif defined(_WINDOWS)
/*******************************************************************************
 *
 * This static method matches the function prototype required by Windows.
 * 
 * It casts the first argument to a Thread pointer and calls the
 * runHandleImpl() method of that object.
 *
 * @return this method always returns 0, the return type is required by
 *     the Windows function prototype
 *
 ******************************************************************************/
DWORD WINAPI Thread::runHandle( LPVOID obj ) 
{
	return (DWORD)(((Thread *)obj)->runHandleImpl());
}

#endif

/*******************************************************************************
 *
 * This method matches the function prototype required by the VxWorks
 * thread class, it casts the first argument to a Thread pointer and calls the
 * init() and run() method of that object.
 *
 * @return this method always returns 0, the return type is required by
 *     the VxWorks function prototype
 *
 ******************************************************************************/
int32_t Thread::runHandleImpl(void)
{
	is_running = true;
	try
	{
		initialize();

		try
		{
			run();
		}
		catch (std::exception& ex)
		{
			printf("Thread::runHandle - unhandled exception during running of thread %s -- %s\n",
				getName().c_str(), ex.what());
		}
		catch (...)
		{
			printf("Thread::runHandle - unhandled exception during running of thread %s\n",
				getName().c_str());
		}
	}
	catch (std::exception& ex)
	{
		printf("Thread::runHandle - unhandled exception during initialization of thread %s -- %s\n",
			getName().c_str(), ex.what());
	}
	catch (...)
	{
		printf("Thread::runHandle - unhandled exception during initialization of thread %s\n",
			getName().c_str());
	}
		
	try
	{
		is_running = false;
		thread_id = THREAD_ID_ERROR;

#if defined (PTHREADS)
		// pthread_cancel((thread_t)thread_id); // is this something that should be done?
#elif defined(VXWORKS)
		taskDelete(taskIdSelf());
#elif defined(_WINDOWS)
		CloseHandle(thread_handle);
#endif

	}
	catch (std::exception& ex)
	{
		printf("Thread::runHandle - unhandled exception during deletion of thread %s -- %s\n",
			getName().c_str(), ex.what());
	}
	catch (...)
	{
		printf("Thread::runHandle - unhandled exception during deletion of thread %s\n",
			getName().c_str());
	}

	try
	{
		finalize();
	}
	catch(...)
	{
	}

	return 0;
}

/*******************************************************************************
 *
 * @return a string representation of this thread
 *
 ******************************************************************************/
std::string Thread::toString()
{
	std::stringstream tmp;
	tmp << "Thread [name=" << getName() << ", id=" << getId() << ", priority="
	    << getPriority() << ", running=" << isRunning() << ", stop_req="
	    << isStopRequested() << "]";

	return tmp.str();
}

} // namespace gsi
