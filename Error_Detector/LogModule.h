

#include "ThreadSafeQueue.hpp"
#include "TestParameters.h"
#include <fstream>
#include <iostream>


typedef unsigned short WCHAR;

class LogModule
{
private:

	std::vector<std::string> _logEntryVector;

	std::fstream _logFileStram;

	std::string _logFileName;

	/*
	*Coordinates of the rectangle on the screen.
	*/
	int _rectXpos = 0;
	int _rectYpos = 0;

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
	void createLogFileTitle(TestParameters testParams)




};