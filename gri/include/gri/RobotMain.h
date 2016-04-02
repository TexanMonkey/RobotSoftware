#pragma once

#include <stdio.h>
#include "gri/Robot.h"
#include "gsi/RobotLogger.h"

int main();

namespace gri
{

class RegisterRobot
{
	public:
	static bool reg;
	static bool registerRobot(const char* name, gri::Robot* r);
	static gri::Robot* getRobot();
	private:
		static gri::Robot* robot;
};
}

#define REGISTER_ROBOT(ROBOT_CLASS)\
gri::Robot* regClass()\
{\
	gri::Robot* r = new ROBOT_CLASS();\
	return r;\
}\
\
bool gri::RegisterRobot::reg = gri::RegisterRobot::registerRobot(#ROBOT_CLASS, regClass());



