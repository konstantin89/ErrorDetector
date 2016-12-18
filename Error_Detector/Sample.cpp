#include <chrono>

class Sample
{
private:
	WCHAR _convertedValue;
	BYTE _chanel;
	std::steady_clock::time_point _sampleTime;

public:	
	Sample(WCHAR value, BYTE chanel, std::steady_clock::time_point sampleTime)
	{
		_convertedValue = value;
		_chanel = chanel;
		_sampleTime = sampleTime;
	}

	~Sample()
	{
		/* EMPTY */
	}

	WCHAR getValue()
	{
		return _convertedValue;
	}

	BYTE getChanel()
	{
		return _chanel;
	}

	/*  TODO calculate time interval between two samples. 
	*/
}