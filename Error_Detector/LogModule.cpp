
#include "LogModule.h"


LogModule::LogModule(char* logFileName)
{
	_logFileName = std::string(logFileName);

	_logFileStram.open(_logFileName.c_str(), std::fstream::out);
	if(_logFileStram.is_open() == false)
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
	std::chrono::time_point<std::chrono::system_clock> nowTime;
	nowTime = std::chrono::system_clock::now();
    auto startTime = std::chrono::system_clock::to_time_t(nowTime);

	_logFileStram << "************** Error Detector Log  Header  **************" << std::endl;
    _logFileStram << "Start time:               " << std::ctime(&startTime) << std::endl;
	_logFileStram << "Player Name:              " << testParams.playerName << std::endl;
	_logFileStram << "Rectangle speed:          " << testParams.rectSpeed << std::endl;
	_logFileStram << "Speed change enabled:     " << testParams.seedChange << std::endl;
	_logFileStram << "Direction change enabled: " << testParams.directionChange << std::endl;
	_logFileStram << "Rectangle Size:           " << testParams.rectSize << std::endl;
	_logFileStram << "Rectangle pattern ID:     " << testParams.rectPatternId << std::endl;
	_logFileStram << "*********************************************************" << std::endl;
}


void LogModule::logEvent(std::string eventDiscription)
{
	_logQueue.push(EVENT_PREFIX + eventDiscription);
}


bool LogModule::tryToPop()
{
	std::string logEntery;
	bool popRetVal = _logQueue.tryToPop(logEntery);
	if(popRetVal)
	{
		writeStrToFile(logEntery);
		return true;
	}
	else
	{
		return false;
	}
}


void LogModule::waitAndPop()
{
	std::string logEntery;
	_logQueue.waitAndPop(logEntery);
}


std::string LogModule::createLogString(WCHAR jPosX, 
	                                   WCHAR jPosY, 
	                                   long sampleDuration)
{
	std::string logEntery;
	logEntery = std::to_string(jPosX) + " " + std::to_string(jPosY);
	logEntery += std::to_string(sampleDuration) + " ";
	logEntery += std::to_string(_rectXpos) + " " + std::to_string(_rectYpos);
	logEntery += std::string("\n");

	return logEntery;
}


int LogModule::writeStrToFile(std::string strToWrite)
{
	if(_logFileStram.is_open() == true)
	{
		_logFileStram << strToWrite << std::endl;
		return SUCCESS;
	}
	else
	{
		return MY_ERROR;
	}
}
