/*******************************************************************************
 *
 * File: PeriodicThread.cpp
 * 	Generic System Interface class for periodic threads
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/PeriodicThread.h"
#include "gsi/Time.h"

static const double MIN_SLEEP_TIME = 0.001;

namespace gsi
{

/*******************************************************************************
 *
 * Constructor for PeriodicTThread
 * 
 * The constructor initializes the instance variables for the task.
 *
 ******************************************************************************/
PeriodicThread::PeriodicThread(std::string name, double period, ThreadPriority priority, 
	uint32_t options, uint32_t stack_size) : Thread(name, priority, options, stack_size)
{
	thread_period = period;
	thread_next_time = 0.0;
}

/*******************************************************************************
 *
 *  Free the resources for this class.
 *
 ******************************************************************************/
PeriodicThread::~PeriodicThread()
{
}

/*******************************************************************************
 *
 * @return the period in seconds
 *
 ******************************************************************************/
double PeriodicThread::getPeriod()
{
	return thread_period;
}

/*******************************************************************************
 *
 * Set the period of this thread.  If the thread is already running, the new
 * period will not have an impact until the current cycle completes.
 * 
 * @param	period	the new period in seconds
 *
 ******************************************************************************/
void PeriodicThread::setPeriod(double period)
{
	thread_period = period;
}

/*******************************************************************************
 *
 * This method implements the base classes run method to periodically call
 * the update() method that must be implemented by subclasses.
 *
 ******************************************************************************/
void PeriodicThread::run(void)
{
	//printf("PeriodicThread:%s,  %s:%d\n", getName().c_str(), __FUNCTION__, __LINE__);
	thread_next_time = Time::getTime();
	
	while( ! isStopRequested() )
	{
		//printf("PeriodicThread:%s,  %s:%d\n", getName().c_str(), __FUNCTION__, __LINE__);
		doPeriodic();
		//printf("PeriodicThread:%s,  %s:%d\n", getName().c_str(), __FUNCTION__, __LINE__);
		
		thread_next_time += thread_period;
		double wait_time = thread_next_time - Time::getTime();

		if (wait_time >= MIN_SLEEP_TIME)
		{
			//printf("PeriodicThread:%s,  %s:%d\n", getName().c_str(), __FUNCTION__, __LINE__);
			sleep(wait_time);
		}
		else
		{
			printf("Value: %.5f   %.5f\n", wait_time, Time::getTime());
			//printf("PeriodicThread:%s,  %s:%d\n", getName().c_str(), __FUNCTION__, __LINE__);
			sleep(MIN_SLEEP_TIME);
		}
	}
}

} // namespace gsi
