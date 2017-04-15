/**********************************************************
*
* @author: Kosta bulgakov
*
* @date: 07-04-2017
*
* @brief: This  file contains implementation of class
*         that represents set of game parameters.
*
**********************************************************/

#ifndef _TEST_PARAMETERS_H
#define _TEST_PARAMETERS_H

#include <fstream>
#include <iostream>
#include <string>


/*************** Defines for return values **************/
#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef MY_ERROR
#define MY_ERROR -1
#endif


/**************** Default Values Defines ****************/
#define DEFAULT_NAME "Default Name"
#define DEFAULT_SPEED 1
#define DEFAULT_DIRECTION_CHANGE 0
#define DEFAULT_SPEED_CHANGE 0
#define DEFAULT_RECT_PATTERN_ID 0
#define DEFAULT_RECT_SIZE 50

#define NUMBER_OF_PARAMETERS 6


class TestParameters
{
public:
	std::string playerName = std::string(DEFAULT_NAME);
	int rectSpeed =        DEFAULT_SPEED;
	int seedChange =       DEFAULT_SPEED_CHANGE;
	int directionChange =  DEFAULT_DIRECTION_CHANGE;
	int rectPatternId =    DEFAULT_RECT_PATTERN_ID;
	int rectSize =         DEFAULT_RECT_SIZE;

	/**
	* Default constructor.
	*/
	TestParameters()
	{
		/* EMPTY */
	}

	/**
	* Constructor.
	* Reads parameters from file.
	*/
	TestParameters(char* parameterFileName)
	{
		readParamsFromFile(parameterFileName);
	}


	/**
	* Constructor.
	* Sets parameters that given by user.
	*/
	TestParameters(	std::string paramPlayerName,
					int         paramPRectSpeed,
					int         paramSpeedChange,
					int         paramDirectionChange,
					int         paramEectPatternId,
					int         paramRectSize)
	{
		playerName = paramPlayerName;
		rectSpeed = paramPRectSpeed;
		seedChange = paramSpeedChange;
		directionChange = paramDirectionChange;
		rectPatternId = paramEectPatternId;
		rectSize = paramRectSize;
	}
				
					

					


	/**
	* Method that used to read parameters from text file.
	*
	*/
	int readParamsFromFile(char* parameterFileName)
	{
		std::ifstream paramFile(parameterFileName);
		if(paramFile.is_open() == false)
		{
			std::cout << "Failed to open parameter file" << std::endl;
			return MY_ERROR;
		}
		else
		{
			std::string parameterArr[NUMBER_OF_PARAMETERS];
			for(int i=0; i<NUMBER_OF_PARAMETERS; i++)
			{
				std::string line;
				std::getline(paramFile, line);
				parameterArr[i] = line;
			}
			paramFile.close();

			playerName =      std::string(parameterArr[0]);
			rectSpeed =       std::stoi(parameterArr[1]);
			seedChange =      std::stoi(parameterArr[2]);
			directionChange = std::stoi(parameterArr[3]);
			rectPatternId =   std::stoi(parameterArr[4]);
			rectSize =        std::stoi(parameterArr[5]);

			return SUCCESS;
		}
	}

	/**
	* Method that used to write parameters to file.
	*
	*/
	int writeParamsToFile(char* parameterFileName)
	{
		std::string parameterArr[NUMBER_OF_PARAMETERS];
		parameterArr[0] = playerName;
		parameterArr[1] = std::to_string(rectSpeed);
		parameterArr[2] = std::to_string(seedChange);
		parameterArr[3] = std::to_string(directionChange);
		parameterArr[4] = std::to_string(rectPatternId);
		parameterArr[5] = std::to_string(rectSize);

		std::ofstream paramFile;
		paramFile.open(parameterFileName, std::ofstream::out);

		if(paramFile.is_open() == false)
		{
			return MY_ERROR;
		}

		paramFile << parameterArr[0] << std::endl;
		paramFile << parameterArr[1] << std::endl;
		paramFile << parameterArr[2] << std::endl;
		paramFile << parameterArr[3] << std::endl;
		paramFile << parameterArr[4] << std::endl;
		paramFile << parameterArr[5] << std::endl;

		paramFile.close();
		return SUCCESS;

	}
};


#endif

/************************* End Of File ****************************/