#include "TestRobot.h"

using namespace gri;

TestRobot::TestRobot()
{
	printf("Initializing Test Robot\n");
	//Test Vector2D
	//copy
	Vector2D* vect = new Vector2D(25.2,3.58);
	Vector2D* clone = new Vector2D(3.0,3.0);
	*vect = *clone;
	printf("Values: (%.3f,%.3f), (%.3f,%.3f)\n",vect->getX(),vect->getY(),clone->getX(),clone->getY());
	delete vect;
	delete clone;
	
	vect = new Vector2D();
	Vector2D* vect_a = new Vector2D(1.0,2.0);
	Vector2D* vect_b = new Vector2D(5.0,3.0);
	Vector2D* vect_c = new Vector2D(2.0,9.0);
	*vect = *vect_a + *vect_b + *vect_c;
	printf("Values: (%.3f,%.3f)\n",vect->getX(),vect->getY());
	*vect = *vect_a - *vect_b - *vect_c;
	printf("Values: (%.3f,%.3f)\n",vect->getX(),vect->getY());
	
}


void TestRobot::disableInit()
{
	printf("TestRobot::%s::%d: Disabled init\n",__INFO__);
}
void TestRobot::disablePeriodic()
{
	printf("TestRobot::%s::%d: Disabled Periodic\n",__INFO__);
}
void TestRobot::teleopInit()
{
	
}
void TestRobot::teleopPeriodic()
{
	
}
void TestRobot::autonomousInit()
{
	
}
void TestRobot::autonomousPeriodic()
{
	
}

REGISTER_ROBOT(TestRobot);
