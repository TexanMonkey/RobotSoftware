/*******************************************************************************
 *
 * File: PeriodicThread.h
 * 	Generic System Interface Periodic Thread class
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "gsi/Thread.h"
#include <stdio.h>

namespace gsi
{

/*******************************************************************************
 *
 * This class extands the Thread class with an implementation of the 
 * run() method that periodically calls a doPeriodic() method that must be 
 * implemented by subclasses.
 * 
 * The period is set when this class is created.  Calls to the doPeriodic() method
 * will be made every <period> seconds as long as the previous call to the
 * update method has completed.  The timing of the next call to update is not 
 * dependant on how long it takes the update method to finish as long as it
 * finishes before the next call should be made.  Each call is made based on 
 * when the init() method was called, how many times doPeriodic() has been called
 * since init() and the period.
 *
 ******************************************************************************/
class PeriodicThread : public Thread
{
	public:
		PeriodicThread(std::string name, double period = 0.01, 
			ThreadPriority priority = PRIORITY_DEFAULT,
			uint32_t options = OPTIONS_DEFAULT, 
			uint32_t stack_size = STACK_SIZE_DEFAULT);

		virtual ~PeriodicThread();

		double getPeriod();
		void setPeriod(double period);
		
	protected:
		virtual void run(void);
		virtual void doPeriodic(void) = 0;

	private:
		double thread_next_time;
		double thread_period;
};

} // namespace gsi
