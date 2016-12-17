
#include <iostream>
#include "AD9772_Comm.h"

#define ADDRESS_AS_GND 0x23

#define CONF_REG_VAL_BIN 0b00111000
#define CONF_REG_VAL_HEX 0x38


int main()
{
	BYTE buff[3];
	
	std::cout << "AD9772 main started." << std::endl;
    AD9772_Comm ad7992Comm(ADDRESS_AS_GND);
    if(ad7992Comm.openCommunicatioBus() == MY_ERROR)
	{
			std::cout << "Open communication failed" << std::endl;
	}
	if(ad7992Comm.initCommunication() == MY_ERROR)
	{
		std::cout << "Init communication failed" << std::endl;
	}
	

	///////////////////////////////////////////////////////////////////////

	
	if(ad7992Comm.setControlRegister(CONFIGURATION_REG, CONF_REG_VAL_HEX) 
		                                                      == MY_ERROR)
	{
		std::cout << "Setting configutation register failed" << std::endl;
	}
	
	///////////////////////////////////////////////////////////////////////

	/*
	* Start the auto sampling mode
	* by setting the sample rating 
	* interval.
	*/
	if(ad7992Comm.setAutoSampleMode(Tx1024) == MY_ERROR)
	{
		std::cout << "Set auto mode failed" << std::endl;
	}


	if(ad7992Comm.setAddrRegister(CONVERTION_RESULT_REG) == MY_ERROR)
	{
		std::cout << "Setting addres register failed" << std::endl;
	}	
	while(1)
	{
		if(ad7992Comm.readRegister(buff, 2) == MY_ERROR)
		{
			std::cout << "read failed" << std::endl;
		}


		WCHAR conVal = buff[1] + (buff[0] & 0x0f) * 256;
		BYTE chanel = (16 & buff[0]) % 15;

		printf("Converted value: %d\n",conVal);
		printf("Ch: %d\n", chanel);
		delay(500);
	}

	return 0;
}
