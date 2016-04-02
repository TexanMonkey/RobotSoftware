/*******************************************************************************
 *
 * File: Thread.h
 *	Generic System Interface Thread wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <exception>
#include <string>

#if defined(PTHREADS)
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#elif defined(VXWORKS) || defined(_WRS_KERNEL)
#include <taskLib.h>

#elif defined(_WINDOWS)
#include <stdint.h>
#include <windows.h>

#else
#pragma warning("Supported platform is not defined")

#endif

namespace gsi
{

/*******************************************************************************
 *
 * This class provides an object oriented way to interact with tasks.  Any
 * class can extend this class and provide an implementation for the run
 * method to operate in a new task.  
 *
 * When the start() method is called, the new thread is created then the 
 * initialize(), run(), and finalize() methods are called from inside of that
 * new thread.  Thus, subclass dynamic resource allocation should take place
 * in the initialize() method, these resources should be released in the
 * finalize method.
 *
 * Subclasses should call isStopRequested() periodically, if stop is requested
 * the subclass should clean up and return from the run() method.
 *
 ******************************************************************************/
class Thread
{
	public:
#if defined(PTHREADS)
		enum ThreadPriority 
		{ 
			PRIORITY_LOWEST		= 4, 
			PRIORITY_LOWER		= 3, 
			PRIORITY_LOW		= 2, 
			PRIORITY_DEFAULT	= 0, 
			PRIORITY_HIGH		= -2, 
			PRIORITY_HIGHER		= -4, 
			PRIORITY_HIGHEST	= -5
		};

		static const uint32_t OPTIONS_DEFAULT		= 0;
		static const uint32_t STACK_SIZE_DEFAULT	= 32768;
        static const int32_t THREAD_ID_ERROR		= -1;

#elif defined(VXWORKS)|| defined(_WRS_KERNEL)
		enum ThreadPriority 
		{ 
			PRIORITY_LOWEST		= 106, 
			PRIORITY_LOWER		= 104, 
			PRIORITY_LOW		= 102, 
			PRIORITY_DEFAULT	= 100, 
			PRIORITY_HIGH		= 98, 
			PRIORITY_HIGHER		= 96, 
			PRIORITY_HIGHEST	= 94 
		};

		static const uint32_t OPTIONS_DEFAULT		= VX_FP_TASK;
		static const uint32_t STACK_SIZE_DEFAULT	= 32768;

#elif defined(_WINDOWS)
		enum ThreadPriority 
		{ 
			PRIORITY_LOWEST		= THREAD_PRIORITY_LOWEST, 
			PRIORITY_LOWER		= THREAD_PRIORITY_LOWEST, 
			PRIORITY_LOW		= THREAD_PRIORITY_BELOW_NORMAL, 
			PRIORITY_DEFAULT	= THREAD_PRIORITY_NORMAL, 
			PRIORITY_HIGH		= THREAD_PRIORITY_ABOVE_NORMAL, 
			PRIORITY_HIGHER		= THREAD_PRIORITY_HIGHEST, 
			PRIORITY_HIGHEST	= THREAD_PRIORITY_HIGHEST 
		};

		static const uint32_t OPTIONS_DEFAULT		= 0; // on Windows, 0 means use the default
        static const uint32_t STACK_SIZE_DEFAULT	= 0; // on Windows, 0 means use the default

#endif

		Thread(std::string name = "_unnamed_thread_", 
			ThreadPriority priority = PRIORITY_DEFAULT,
			uint32_t options = OPTIONS_DEFAULT, 
			uint32_t stack_size = STACK_SIZE_DEFAULT);

		virtual ~Thread(void);

		void start(void);
		virtual void requestStop(void);
		
		bool isRunning(void);
		bool isStopRequested(void);

		void setPriority(ThreadPriority priority);
		ThreadPriority getPriority(void);

		void setStackSize(uint32_t size);
		uint32_t getStackSize(void);
		
		void setOptions(uint32_t options);
		void addOptions(uint32_t options);
		uint32_t getOptions(void);
		
		int64_t getId(void);
		std::string getName(void);

		virtual std::string toString(void);
		
		static void sleep(double seconds);

	protected:
		virtual void initialize(void) {}
		virtual void run(void) = 0;
		virtual void finalize(void) {}

	private:
#if defined (PTHREADS)
        static void * runHandle(void * val);

#elif defined(VXWORKS)
		static int runHandle(uint32_t val, ...);

#elif defined(_WINDOWS)
		static DWORD WINAPI runHandle( LPVOID obj );
		HANDLE thread_handle;

#endif

		int32_t runHandleImpl(void);
		void postRunHandle(void);
		
		std::string thread_name;
		ThreadPriority thread_priority;
		uint32_t thread_options;
		uint32_t thread_size;
		
		bool is_running;
		bool is_stop_requested;

		int64_t thread_id;
};

} // namespace gsi
