
#include "LogModule.h"


std::string LOG_FILE_NAME_STR("./LogFileTest");

int main()
{
	printf("Starting Log File Tests\n");

	TestParameters testParams;
	LogModule logMod(LOG_FILE_NAME_STR.c_str());

	logMod.createLogFileTitle(testParams);

	logMod.pushLogEvent("SOME EVENT");

	logMod.setRectCoordinates(14, 67);

	std::string logEntery = logMod.createLogString(10,10, 100);
	logMod.pushLogEntery(logEntery);

	while(logMod.tryToPop())
	{
		/* EMPTY */
	}

	return 0;
}