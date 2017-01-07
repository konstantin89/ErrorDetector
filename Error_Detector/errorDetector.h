/**************************************************
*
* @author: Kosta Bulgakov
*
* @date: 15/12/2016
*
* @brief: This file contains header file for
*         real time error detection application.
*
***************************************************/

/*
* Delay thread for specific ammount of microsecs.
*
*
*  auto sampleT = std::chrono::microseconds
*                 (NTERVAL_MICROSEC);
*  std::this_thread::sleep_for(sampleT);
*/

#include "ThreadSafeQueue.hpp"
#include "TerminateFlag.hpp"
#include "../AD9772_Comm_Module/AD9772_Comm.h"
#include "Sample.h"


#include <thread>
#include <chrono>
#include <limits>
#include <random>
#include <iostream>
#include <vector>
#include <fstream>
#include <exception>
#include <signal.h>
#include <string>


/************************* For Debug  ************************/

#define DEBUG 0


#define print_debug(val, chanel)\
            do{ if(DEBUG) printf("Value:%d, Chanel:%d\n", val, chanel);}\
                                                                  while(0)


#define print_error(errorText)\
            fprintf(stderr, errorText);


/******************* Joystick calibration values ***********/

#define X_MIN 16
#define X_MID 2056
#define X_MAX 4095

#define Y_MIN 20
#define Y_MID 2048
#define Y_MAX 4095


/************************** AD7992  ***********************/

#define ADDRESS_AS_GND 0x23

#define CONF_REG_VAL_HEX 0x38


/******************* Time intervals **********************/

/*
* Time in secs to perform test.
*/
#define TEST_DURATION_SECS 5

/*
* Delay between the termination command to return in main().
*/
#define FINISH_DELAY_SEC 2


/********************** Other ****************************/

/*
* Check the termination flag status.
*/
#define TERMINATION_FLAG gTerminateFlag.checkFlag()

/*
* Zero sample.
*/
#define ZERO_SAMPLE 0
#define ZERO_SAMPLE_STR "0 0"

//Do not ommit LSB bits from samples.
#define RESULT_MASK  0xfff 





/******************* End Of File *************************/