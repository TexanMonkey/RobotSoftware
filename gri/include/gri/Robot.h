#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "gsi/PeriodicThread.h"
#include "gri/tinyxml2.h"


//Defines
#define __INFO__ __FUNCTION__,__LINE__

#define GET_XML_FLOAT(PARENT,CHILD,VALUE,DEFUALT)\
XMLElement* child_##CHILD = PARENT->FirstChildElement(#CHILD);\
if(child_##CHILD == NULL)\
{\
	printf("Element \"%s\" not found\n",#CHILD);\
	VALUE = DEFUALT;\
}\
else\
{\
	VALUE = atof(child_##CHILD->GetText());\
}

#define GET_XML_INT(PARENT,CHILD,VALUE,DEFUALT)\
XMLElement* child_##CHILD = PARENT->FirstChildElement(#CHILD);\
if(child_##CHILD == NULL)\
{\
	printf("Element \"%s\" not found\n",#CHILD);\
	VALUE = DEFUALT;\
}\
else\
{\
	VALUE = atoi(child_##CHILD->GetText());\
}


//Defualts
#define DEFUALT_MAIN_THREAD_PERIOD 0.05
#define DEFUALT_MAIN_THREAD_PRIORITY 0


namespace gri
{
	
using namespace tinyxml2;


class Robot: public gsi::PeriodicThread
{
	public:
		Robot();
		virtual void disableInit() = 0;
		virtual void disablePeriodic() = 0;
		virtual void teleopInit() = 0;
		virtual void teleopPeriodic() = 0;
		virtual void autonomousInit() = 0;
		virtual void autonomousPeriodic() = 0;
		
		enum Mode {DISABLED = 0, TELEOP, AUTONOMOUS, EMERGANCY_STOP};
	protected:
		void doPeriodic();
		XMLDocument* settings_file;
	private:
		Mode mode;
		bool reinit;
		bool settings_file_exists;
		void loadSettings(XMLDocument* doc);
};
}





