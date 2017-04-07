

#include "ThreadSafeQueue.hpp"
#include "TestParameters.h"
#include <fstream>



typedef unsigned short WCHAR;

class LogModule
{
private:

	std::vector<std::string> logEntryVector;

	std::fstream logFileStram;

	/*
	*Coordinates of the rectangle on the screen.
	*/
	int rectXpos = 0;
	int rectYpos = 0;

public:

	/**
	*
	*
	*/
	LogModule(char* logFileName);

	/**
	*
	*
	*/
	~LogModule();

	/**
	*
	*
	*/
	void setRectCoordinates(int x, int y);

	/**
	*
	*
	*/
	void printLogEnteryToFile();


	/**
	*
	*
	*/
	std::string createLogString(WCHAR, jPosX, WCHAR jPosY, long sampleDuration);

	/**
	*
	*
	*/
	void logEvent(std::string eventDiscription);

	/**
	*
	*
	*/
	int createLogFileTitle(TestParameters testParams)




};