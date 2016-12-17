
#include "AD9772_Comm.h"


AD9772_Comm::AD9772_Comm(int devAddress) : A2D_Comm_I2C(devAddress)
{
	/* EMPTY */
}


int AD9772_Comm::setControlRegister(BYTE regAddress, BYTE regValue)
{
	int writeRetVal;
	BYTE buffer[2];
	
	buffer[0] = regAddress;
	buffer[1] = regValue;
	writeRetVal = writeData(buffer, 2);

	if(writeRetVal == 2)
	{
		return SUCCESS;
	}
	else
	{
		return MY_ERROR;
	}
}


int AD9772_Comm::setAddrRegister(BYTE regAddress)
{
	int writeRetVal;
	BYTE buffer[1];

	buffer[0] = regAddress;

	writeRetVal = writeData(buffer, 1);

	if(writeRetVal == 1)
	{
		return SUCCESS;
	}
	else
	{
		return MY_ERROR;
	}
}

int AD9772_Comm::readRegister(BYTE* retBuf, int bytesToRead)
{
	return readData(retBuf, bytesToRead);
}


int AD9772_Comm::setAutoSampleMode(ConversRate rate)
{
	return setControlRegister(CYCLE_TIMER_REGISTER, ConvertionRateMap[rate]);
}

int AD9772_Comm::setHighSpeedMode()
{
	int writeRetVal;
	BYTE buf[1];

	//High speed command: 00001XXX, 
	//For AD9772-1 with AS=GRD we have XXX=011
	//00001011 binary is 1+2+8 = 11
	buf[0] = 11;
	writeRetVal = writeData(buf, 1);

	return writeRetVal;
}

int AD9772_Comm::setCommandMode(BYTE chanelToSample)
{
	int writeRetVal;
	BYTE buf[1];
	buf[0] = chanelToSample;
	writeRetVal = writeData(buf, 1);
	return writeRetVal;
}


