
#include "LogModule.h"


LogModule::LogModule(char* logFileName)
{
	_logFileName = std::string(logFileName);

	_logFileStram.open(_logFileName.c_str(), std::fstream::out);
	if(logFileStram.is_open() == false)
	{
		std::cout << "Failed to open log file." << std::endl;
	}
}

LogModule::~LogModule()
{
	_logFileStram.close();
}


void LogModule::setRectCoordinates(int x, int y)
{
	_rectXpos = x;
	_rectYpos = y;
}


void LogModule::createLogFileTitle(TestParameters& testParams)
{
	_logFileStram << "************** Error Detector Log  Header  **************" << std::endl;

	std::chrono::time_point<std::chrono::system_clock> nowTime;
    nowTime = std::chrono::system_clock::now();
    auto startTime = std::chrono::system_clock::to_time_t(nowTime);
    _logFileStram << "Start time:               " << std::ctime(&startTime) << std::endl;

	_logFileStram << "Player Name:              " << testParams.playerName << std::endl;
	_logFileStram << "Rectangle speed:          " << testParams.rectSpeed << std::endl;
	_logFileStram << "Speed change enabled:     " << testParams.seedChange << std::endl;
	_logFileStram << "Direction change enabled: " << testParams.directionChange << std::endl;
	_logFileStram << "Rectangle pattern ID:     " << testParams.rectPatternId << std::endl;

	_logFileStram << "*********************************************************" << std::endl;

}