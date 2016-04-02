#include "gri/Robot.h"


namespace gri
{
Robot::Robot(): gsi::PeriodicThread("MainRobotThread",0.01)
{
	//printf("Robot\n");
	reinit = true;
	mode = DISABLED;
	FILE* s_file = fopen("RobotSettings.xml","r");
	if(s_file == NULL)
	{
		printf("Robot::%s::%d: Settings file doesn't exist\n",__INFO__);
		settings_file_exists = false;
		settings_file = NULL;
	}
	else
	{
		settings_file = new XMLDocument();
		settings_file->LoadFile(s_file);
		fclose(s_file);
		if(settings_file->Error())
		{
			printf("Robot::%s::%d: Error loading file\n",__INFO__);
			settings_file_exists = false;
		}
		else
		{
			loadSettings(settings_file);
			settings_file_exists = true;
		}
	}
}

void Robot::loadSettings(XMLDocument* doc)
{
	printf("Robot::%s::%d: Loading Settings...\n",__INFO__);
	XMLElement* robot = doc->FirstChildElement("Robot");
	if(robot != NULL)
	{
		XMLElement* main_thread = robot->FirstChildElement("MainThread");
		if(main_thread != NULL)
		{
			double period = 0.0;
			int priority = 0;
			GET_XML_FLOAT(main_thread,Period,period,DEFUALT_MAIN_THREAD_PERIOD);
			setPeriod(period);
			GET_XML_FLOAT(main_thread,Priority,priority,DEFUALT_MAIN_THREAD_PRIORITY);
			setPriority(static_cast<gsi::Thread::ThreadPriority>(priority));
			//printf("Period is: %.3f\n",period);
			//printf("Values: %s\n",main_thread->FirstChildElement("Period")->GetText());
			//setPeriod()
		}
		else
		{
			setPeriod(DEFUALT_MAIN_THREAD_PERIOD);
			setPriority(static_cast<gsi::Thread::ThreadPriority>(DEFUALT_MAIN_THREAD_PRIORITY));
		}
	}
	else
	{
		printf("Robot::%s::%d: Error, no robot element\n",__INFO__);
		printf("Robot::%s::%d: Assigning defualt settings\n",__INFO__);
	}
}

void Robot::doPeriodic()
{
	switch(mode)
	{
		case DISABLED:
		{
			if(reinit == true)
			{
				disableInit();
				reinit = false;
			}
			disablePeriodic();
		}break;
		case TELEOP:
		{
			if(reinit == true)
			{
				teleopInit();
				reinit = false;
			}
			teleopPeriodic();
		}break;
		case AUTONOMOUS:
		{
			if(reinit == true)
			{
				autonomousInit();
				reinit = false;
			}
			autonomousPeriodic();
		}break;
		defualt:
		{
			
		}break;
	}
}


}



