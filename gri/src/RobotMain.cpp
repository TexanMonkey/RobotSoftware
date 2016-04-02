#include "gri/RobotMain.h"

int main()
{
	printf("Initializing Robot\n");
	gri::Robot* robot = gri::RegisterRobot::getRobot();
	if(robot == NULL)
	{
		printf("ERROR: No Robot class registered!\n");
		printf("Robot Terminating\n");
		return 1;
	}
	
	robot->start();
	while(!robot->isRunning());
	while(robot->isRunning())
	{
		gsi::Thread::sleep(0.1);
	}
	
	return 0;
}



namespace gri
{
Robot* RegisterRobot::robot = NULL;
gri::Robot* RegisterRobot::getRobot()
{
	return RegisterRobot::robot;
}

bool RegisterRobot::registerRobot(const char* name, gri::Robot* r)
{
	printf("Registering Robot \"%s\"\n",name);
	RegisterRobot::robot = r;
}


}

