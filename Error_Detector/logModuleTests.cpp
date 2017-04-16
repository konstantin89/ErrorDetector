
#include "LogModule.h"


std::string LOG_FILE_NAME_STR("./LogFileTest");
std::string PARAM_FILE_NAME_STR("./ParametersTest");

int main()
{
	printf("Starting Log File Tests\n");

	TestParameters testParams;

	if(TestParameters::paramFileExists(PARAM_FILE_NAME_STR.c_str()))
	{
		testParams.readParamsFromFile(PARAM_FILE_NAME_STR.c_str());
		std::cout << testParams.getParamsAsStr();
	}
	else
	{
		testParams.writeParamsToFile(PARAM_FILE_NAME_STR.c_str());
		std::cout << testParams.getParamsAsStr();
	}


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