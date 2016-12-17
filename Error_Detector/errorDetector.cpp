/*************************************************
*
* @author: Kosta Bulgakov
*
* @date: 02/09/2016
*
* @brief: This file contains multithreaded real time 
*         system for human error detection.
*
*************************************************/

#include "errorDetector.h"

//Queues for samples to be processed.
ThreadSafeQueue<WCHAR> gSampleQueueX;
ThreadSafeQueue<WCHAR> gSampleQueueY;

//Queue for samples to be printed to text file.
ThreadSafeQueue<std::string> gRawDataForFileWriting;

//Conter of the number of samples that were made.
long gSampleCountX = 0;
long gSampleCountY = 0;

//Flag that is turned on when we get termination command.
TerminateFlag gTerminateFlag;

//Signal action fot ctrl+c.
struct sigaction gSigIntHandler;


/*
* Run automatic sampling mode
* in AD9772 manager thread.
*
*/
void autoSamplingMode()
{
	BYTE buff[4];

    AD9772_Comm ad7992Comm(ADDRESS_AS_GND);
    if(ad7992Comm.openCommunicatioBus() == MY_ERROR)
    {
        print_error("Open communication failed\n");
    }
    if(ad7992Comm.initCommunication() == MY_ERROR)
    {
        print_error("Init communication failed\n")
    }
    
    if(ad7992Comm.setControlRegister(CONFIGURATION_REG, CONF_REG_VAL_HEX) 
                                                              == MY_ERROR)
    {
        print_error("Setting configutation register failed\n");
    }
    /*
    * Start the auto sampling mode
    * by setting the sample rating 
    * interval.
    */
    if(ad7992Comm.setAutoSampleMode(Tx32) == MY_ERROR)
    {
        print_error("Set auto mode failed\n");
    }
    if(ad7992Comm.setAddrRegister(CONVERTION_RESULT_REG) == MY_ERROR)
    {
        print_error("Setting addres register failed\n");
    }
    
    WCHAR conValX = 0;
    WCHAR conValY = 0;
    BYTE chanel = 0;
    while(TERMINATION_FLAG == false)
    {
		
		while(chanel != CHANEL_1)
        {
            if(ad7992Comm.readRegister(buff, 2) == MY_ERROR)
            {
                std::cout << "read failed" << std::endl;
            }
            conValX = buff[1] + (buff[0] & 0x0f) * 256;
            conValX = conValX & RESULT_MASK;
            chanel = (16 & buff[0]) % 15;
            print_debug(conValX, chanel);
        }
        gSampleQueueX.push(conValX);
        gSampleCountX++;
        
        while(chanel != CHANEL_2)
        {
            if(ad7992Comm.readRegister(buff, 2) == MY_ERROR)
            {
                std::cout << "read failed" << std::endl;
            }
            conValY = buff[1] + (buff[0] & 0x0f) * 256;
            conValY = conValY & RESULT_MASK;
            chanel = (16 & buff[0]) % 15;
            print_debug(conValY, chanel);
        }
        gSampleQueueY.push(conValY);
        gSampleCountY++;
    
        /*
        * Save samples to queue that will be written to file
        * for further processing.
        */
        std::string sampleStr = std::to_string(conValX) + " " +
                                std::to_string(conValY);
        gRawDataForFileWriting.push(sampleStr);
	}
    return;
}


void commandModeOneChan(int chanelToSample)
{
    BYTE buff[5];
    
    AD9772_Comm ad7992Comm(ADDRESS_AS_GND);
    if(ad7992Comm.openCommunicatioBus() == MY_ERROR)
    {
            std::cout << "Open communication failed" << std::endl;
    }
    if(ad7992Comm.initCommunication() == MY_ERROR)
    {
        std::cout << "Init communication failed" << std::endl;
    }
    
    if(ad7992Comm.setControlRegister(CONFIGURATION_REG, CONF_REG_VAL_HEX) 
                                                              == MY_ERROR)
    {
        std::cout << "Setting configutation register failed" << std::endl;
    }
    if(ad7992Comm.setCommandMode(chanelToSample) == MY_ERROR)
    {
        std::cout << "Setting command mode failed" << std::endl;
    } 

    WCHAR conVal = 0;
    BYTE chanel = 0;
    while(TERMINATION_FLAG == false)
    {
        if(ad7992Comm.setCommandMode(chanelToSample) == MY_ERROR)
        {
            std::cout << "Setting command mode failed" << std::endl;
        } 
        if(ad7992Comm.readRegister(buff, 2) == MY_ERROR)
        {
            std::cout << "read failed" << std::endl;
        }
        conVal = buff[1] + (buff[0] & 0x0f) * 256;
        chanel = (16 & buff[0]) % 15;
        print_debug(conVal, chanel);

	gSampleCountX++;
	std::string sampleStr = std::to_string(conVal) + " " +
                                std::to_string(0);
        gRawDataForFileWriting.push(sampleStr);
    }
    return;
}

void commandModeTwoChan()
{
    BYTE buff[4];
    
    AD9772_Comm ad7992Comm(ADDRESS_AS_GND);
    if(ad7992Comm.openCommunicatioBus() == MY_ERROR)
    {
            std::cout << "Open communication failed" << std::endl;
    }
    if(ad7992Comm.initCommunication() == MY_ERROR)
    {
        std::cout << "Init communication failed" << std::endl;
    }
    
    if(ad7992Comm.setControlRegister(CONFIGURATION_REG, CONF_REG_VAL_HEX) 
                                                              == MY_ERROR)
    {
        std::cout << "Setting configutation register failed" << std::endl;
    }
    if(ad7992Comm.setCommandMode(COMMAND_BOTH_CH) == MY_ERROR)
    {
        std::cout << "Setting command mode failed" << std::endl;
    } 

    WCHAR conValX = 0;
    WCHAR conValY = 0;
    BYTE chanel = 0;
    while(TERMINATION_FLAG == false)
    {	

        if(ad7992Comm.setCommandMode(COMMAND_CH_1) == MY_ERROR)
        {
            std::cout << "Setting command mode failed" << std::endl;
        } 
	
	    if(ad7992Comm.readRegister(buff, 2) == MY_ERROR)
        {
            std::cout << "read failed" << std::endl;
        }
        else
        {
            conValX = buff[1] + (buff[0] & 0x0f) * 256;
            chanel = (16 & buff[0]) % 15;
	        gSampleCountX++;
        }
        print_debug(conValX, chanel);


	    if(ad7992Comm.setCommandMode(COMMAND_CH_2) == MY_ERROR)
        {
            std::cout << "Setting command mode failed" << std::endl;
        } 
        if(ad7992Comm.readRegister(buff, 2) == MY_ERROR)
        {
            std::cout << "read failed" << std::endl;
        }
        else
        {
            conValY = buff[1] + (buff[0] & 0x0f) * 256;
            chanel = (16 & buff[0]) % 15;
	        gSampleCountY++;
        }
        print_debug(conValY, chanel);


	    std::string sampleStr = std::to_string(conValX) + " " +
                                std::to_string(conValY);
        gRawDataForFileWriting.push(sampleStr);
        
    }
    return;
}

/*
* Function that used in the thread that manages the
* interface with AD9772 device.
*
*/
void AD9772_Manager()
{
    //autoSamplingMode();
    //commandModeOneChan(COMMAND_CH_1);
    commandModeTwoChan();

    std::cout << "AD9772_Manager Done." << std::endl;
    return;
}

/**
* Function that used in thread that processes the
* input from AD9772 device.
*
*/
void dataProcessor()
{
    bool popRetValX;
    bool popRetValY;
    while((TERMINATION_FLAG == false) or\
          (gSampleQueueX.empty() == false))
    {
        WCHAR popedValX;
        WCHAR popedValY;
        if(TERMINATION_FLAG == true)
        {
            popRetValX = gSampleQueueX.tryToPop(popedValX);
            popRetValY = gSampleQueueY.tryToPop(popedValY);
            if ((popRetValX == false) && (popRetValY == false))
                continue; 
        }        
        else
        {
            gSampleQueueX.waitAndPop(popedValX);
            gSampleQueueY.waitAndPop(popedValY);
            /*
            * DO CALCULATION ON DATA HERE.
            */
        }
    }
    std::cout << "dataProcessor Done." << std::endl;
    return;
}

/**
* Function used for writing samples of GPIO to
* text file.
*/
void dataWriter(const char* fileName)
{
    std::fstream fs;
    std::string popedStr;
    bool popRetVal;
    try
    {
        fs.open(fileName, std::fstream::out);

        std::chrono::time_point<std::chrono::system_clock> nowTime;
        nowTime = std::chrono::system_clock::now();
        std::time_t startTime = \
                    std::chrono::system_clock::to_time_t(nowTime);
        fs << "Start time: " << std::ctime(&startTime);
    }
    catch(std::exception& e)
    {
        std::cout << "Exception in dataWriter: " << e.what();
        if(fs)
        {
            fs.close();
        }
    }
    while((TERMINATION_FLAG == false) or\
          (gRawDataForFileWriting.empty() == false))

    {
        if(TERMINATION_FLAG == true)
        {
            popRetVal = gRawDataForFileWriting.tryToPop(popedStr);
            if(popRetVal == false)
                continue;
        }
        else
        {
            gRawDataForFileWriting.waitAndPop(popedStr);
        }
        
        fs << popedStr << std::endl;
    }
    fs.close();
    std::cout << "dataWriter Done." << std::endl;
    return;
}

/*
* Function that used to pass termination signal to the threads
* of the process, and make the threads return safely.
*/
void terminateThreads()
{
    //Set the termination flag to true.
    gTerminateFlag.setFlag();
    //Notify all the CV of the thread safe queues.
    gSampleQueueX.notifyAll(ZERO_SAMPLE);
    gSampleQueueY.notifyAll(ZERO_SAMPLE);

    gRawDataForFileWriting.notifyAll(ZERO_SAMPLE_STR);

}

/*
* Function that used as the signal handler
* for the Ctrl+c termination command.
*/
void sigIntHandler(int sigNum)
{
    std::cout << "\nSignal handler called with signal number: " \
              << sigNum << std::endl;

    std::cout << "Data queueX:" <<gSampleQueueX.size() << std::endl;         
    std::cout << "Data queueY:" <<gSampleQueueY.size() << std::endl;    
    terminateThreads();

    std::cout<< "File writing queue: "<<gRawDataForFileWriting.size()
             << std::endl;

    if(sigNum != SIGINT)
    {
        std::cout << "Total sample countX: " << gSampleCountX << std::endl;
        std::cout << "Sample per second rateX: " \
                  << int(gSampleCountX/TEST_DURATION_SECS) << std::endl;

        std::cout << "Total sample countY: " << gSampleCountY << std::endl;
        std::cout << "Sample per second rateY: " \
                  << int(gSampleCountY/TEST_DURATION_SECS) << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(FINISH_DELAY_SEC));
    exit(0);
}

/*
* Set signal handler for Ctrl+c termination
* signal.
*/
void setSignalHandler()
{
    gSigIntHandler.sa_handler = sigIntHandler;
    sigemptyset(&gSigIntHandler.sa_mask);
    gSigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &gSigIntHandler, NULL);
}

/*
* main()
*/
int main()
{   
    
    const char* fileNameX = "../Samples_From_A2D";

    setSignalHandler();

    std::thread AD9772_ManagerThread(AD9772_Manager);
    std::thread dataProcessorThread(dataProcessor);
    std::thread dataWriterThread(dataWriter,fileNameX);

    AD9772_ManagerThread.detach();
    dataProcessorThread.detach();
    dataWriterThread.detach();

    std::this_thread::sleep_for(std::chrono::seconds(TEST_DURATION_SECS));
    sigIntHandler(0);
    return 0;
}























