/*****************************************************************
*
* @author: Kosta Bulgakov
*
* @date: 19\12\2016
*
* @brief: Minimal code for interfacing with AD7992 that
*         used to know the maximum sample reading rate.
*
*         The code contains long functions that made with 
*         bad coding style that used only to find the 
*         maximum possible reading rate of samples from AD7992.
*
*****************************************************************/


/*********************** Includes *******************************/
#include <iostream>
#include "AD9772_Comm.h"
#include <chrono>

/**************** Defines for AD7992 device  *******************/
// Flag for debugging.
#define DEBUG 0

// Print samples to standard output for debugging. 
#define print_debug(val, chanel)\
            do{ if(DEBUG) printf("Value:%d, Chanel:%d\n", val, chanel);} while(0)

//Print errror to user.
#define print_error(errorText)\
            fprintf(stderr, errorText);


#define ADDRESS_AS_GND 0x23
#define CONF_REG_VAL_HEX 0x38

//Do not ommit LSB bits from samples.
#define RESULT_MASK  0xfff 


/*
* Function that used to run AD7992 device in command
* mode and read samples from it.
*
* The function made to count the sample per second rate
* of the device when running the minimum needed code.
* 
* This test used to chech how the error detector application
* reading rate is close to the optimal rate that is counted
* in this program.
*
*/
int commandModeTwoCh();


int main()
{
	commandModeTwoCh();
	return SUCCESS;
}


int commandModeTwoCh()
{
	long sampleCount = 0;
    BYTE buff[4];
    int fileDescriptor = 0;
    UINT writenByteNum = 0;

    /*
    * Get comminication descriptor to I2C device
    * from linux.
    */
	char I2C_Device_Name[40] = "/dev/i2c-1";
	fileDescriptor = open(I2C_Device_Name, O_RDWR);
	if(fileDescriptor < 0)
	{
		print_error("open() failed\n");
		return MY_ERROR;
	}

    if(ioctl(fileDescriptor, I2C_SLAVE, ADDRESS_AS_GND) < 0)
	{
		print_error("ioctl() failed. Failed to gain bus access\n");
		return MY_ERROR;
	}

	/*
	* Configure the AD7992 device.
	*/
    BYTE bytesToWrite = 2;
    buff[0] = CONFIGURATION_REG;
    buff[1] = CONF_REG_VAL_HEX;
	if(bytesToWrite < 1)
	{
		return SUCCESS;
	}
	writenByteNum = write(fileDescriptor, buff, bytesToWrite);	
	if(writenByteNum < 1)
	{
		print_error("write failed\n");
		return MY_ERROR;
	}


    WCHAR conVal = 0;
    BYTE chanel = 0;
    UINT bytesReaded = 0;

    /*
    * Save the start time of the sampling.
    */
    std::chrono::steady_clock::time_point start = 
                                       std::chrono::steady_clock::now();
    while(1)
    {	
    	/*
    	* Ask the device to sample chanel 1 
    	* and read the sample.
    	*/
	    bytesToWrite = 1;
	    buff[0] = 0x10;
	    writenByteNum = write(fileDescriptor, buff, bytesToWrite);	
		if(writenByteNum < 1)
		{
			print_error("write failed\n");
			return MY_ERROR;
		}

		bytesReaded = read(fileDescriptor, buff, 2);
		if(bytesReaded < 1)
		{
			print_error("failed to read\n");
			return MY_ERROR;
		}
		conVal = buff[1] + (buff[0] & 0x0f) * 256;
        chanel = (16 & buff[0]) % 15;   
        print_debug(conVal, chanel);

        /*
    	* Ask the device to sample chanel 2
    	* and read the sample.
    	*/
        bytesToWrite = 1;
	    buff[0] = 0x20;
	    writenByteNum = write(fileDescriptor, buff, bytesToWrite);	
		if(writenByteNum < 1)
		{
			print_error("write failed\n");
			return MY_ERROR;
		}

		bytesReaded = read(fileDescriptor, buff, 2);
		if(bytesReaded < 1)
		{
			print_error("failed to read\n");
			return MY_ERROR;
		}
		conVal = buff[1] + (buff[0] & 0x0f) * 256;
        chanel = (16 & buff[0]) % 15;
        print_debug(conVal, chanel);

        sampleCount++;
        std::chrono::steady_clock::time_point end = 
                                              std::chrono::steady_clock::now();

         long microsecsPassed = 
              std::chrono::duration_cast<std::chrono::microseconds>
                                                         (end - start).count();

        if(microsecsPassed >= 1000000) 
        	break;
        
    }
    std::cout << "Took " << sampleCount << " samples from each chanel" 
                                           << std::endl;
    return SUCCESS;
}



/****************************** End of file ******************************/