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

//Queue for samples to be processed.
ThreadSafeQueue<PSample> gSampleQueue;

//Queue for samples to be printed to text file.
ThreadSafeQueue<std::string> gRawDataForFileWriting;

//Conter of the number of samples that were made.
long sampleCount = 0;

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
    if(ad7992Comm.setAutoSampleMode(Tx64) == MY_ERROR)
    {
        print_error("Set auto mode failed\n");
    }

    if(ad7992Comm.setAddrRegister(CONVERTION_RESULT_REG) == MY_ERROR)
    {
        print_error("Setting addres register failed\n");
    }
    
    WCHAR ch1Val, ch2Val;
    while(TERMINATION_FLAG == false)
    {
         //std::chrono::steady_clock::time_point timeSampleTaken = std::chrono::steady_clock::now();

        if(ad7992Comm.readAutoMode(ch1Val, ch2Val, ADDRESS_AS_GND) == MY_ERROR)
        {
            continue;
        }
        else
        {
            sampleCount++;
            print_debug(ch1Val, 0);
            print_debug(ch2Val, 1);
                    std::string sampleStr = std::to_string(ch1Val) + 
                                " " + std::to_string(ch2Val);
            gRawDataForFileWriting.push(sampleStr);
        }


	}
    return;
}


void commandModeTwoChan()
{
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

    /*
    * Time points that used to calculate the time
    * between samples.
    */
    std::chrono::steady_clock::time_point timeSampleTaken;

    /*
    * Sampling loop.
    */
    while(TERMINATION_FLAG == false)
    {	
        WCHAR ch1Val, ch2Val;
       
	    timeSampleTaken = std::chrono::steady_clock::now();

	    if(ad7992Comm.readCommandMode(ch1Val, ch2Val, ADDRESS_AS_GND) 
                                                            == MY_ERROR)
        {
            continue;
        }
        else
        {
            sampleCount++;

            print_debug(ch1Val, 0);
            print_debug(ch2Val, 1);

            PSample newSample = new Sample(ch1Val, ch2Val, 
                                                   timeSampleTaken);
            gSampleQueue.push(newSample);
            
            //Save the sample as string to be written to text file.
	        std::string sampleStr = std::to_string(ch1Val) + 
                                   " " + std::to_string(ch2Val);
            gRawDataForFileWriting.push(sampleStr);
        }
        
    }
    return;
}



/*
* Function that used in the thread that manages the
* communication with AD9772 device.
*
*/
void AD9772_Manager()
{
    //autoSamplingMode();
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
    int popRetVal;

    while((TERMINATION_FLAG == false) or\
          (gSampleQueue.empty() == false))
    {
        PSample popedSample;
        if(TERMINATION_FLAG == true)
        {
            popRetVal = gSampleQueue.tryToPop(popedSample);
            if (popRetVal == false)
                continue; 
        }        
        else
        {
            gSampleQueue.waitAndPop(popedSample);
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
    gSampleQueue.notifyAll(NULL);

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

    std::cout << "Data queue:" <<gSampleQueue.size() << std::endl;        
    terminateThreads();

    std::cout<< "File writing queue: "<<gRawDataForFileWriting.size()
             << std::endl;

    if(sigNum != SIGINT)
    {
        std::cout << "Total sample count: " << sampleCount << std::endl;
        std::cout << "Sample per second rate: " \
                  << int(sampleCount/TEST_DURATION_SECS) << std::endl;
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
    
    const char* fileNameX = "./Samples_From_A2D";

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



///////////////////////////////////////////////////////////////

// void commandModeTwoChanV2()
// {
//     BYTE buff[4];
//     WCHAR conValX = 0;
//     WCHAR conValY = 0;
//     BYTE chanel = 0;

    
//     AD9772_Comm ad7992Comm(ADDRESS_AS_GND);
//     if(ad7992Comm.openCommunicatioBus() == MY_ERROR)
//     {
//             std::cout << "Open communication failed" << std::endl;
//     }
//     if(ad7992Comm.initCommunication() == MY_ERROR)
//     {
//         std::cout << "Init communication failed" << std::endl;
//     }
    
//     if(ad7992Comm.setControlRegister(CONFIGURATION_REG, CONF_REG_VAL_HEX) 
//                                                               == MY_ERROR)
//     {
//         std::cout << "Setting configutation register failed" << std::endl;
//     }
//     if(ad7992Comm.setCommandMode(COMMAND_BOTH_CH) == MY_ERROR)
//     {
//         std::cout << "Setting command mode failed" << std::endl;
//     } 

//     /*
//     * Time points that used to calculate the time
//     * between samples.
//     */
//     std::chrono::steady_clock::time_point timeSampleTakenX;
//     std::chrono::steady_clock::time_point timeSampleTakenY;

//     int desc = ad7992Comm.getDesc();

//     /*
//     * Sampling loop.
//     */
//     while(TERMINATION_FLAG == false)
//     {   

//         struct i2c_rdwr_ioctl_data ioctlMsg;
//         struct i2c_msg msg [2];
        
//         BYTE writeBuf[1];
//         BYTE readBuf [4];
        
//         writeBuf[0] = 0x30;
        
//         //msg[0].addr = DEV_ADDR;
//         msg [0].flags = 0; //write
//         msg [0].len = 1;
//         msg [0].buf = writeBuf;
        
//         //msg[1].addr = DEV_ADDR;
//         msg [1].flags = 1; //?read?
//         msg [1].len = 4;
//         msg [1].buf = readBuf;
        
//         ioctlMsg.msgs = msg;
//         ioctlMsg.nmsgs = 2;
        
//         if(ioctl(desc,  I2C_RDWR, &ioctlMsg) < 0)
//         {
//           std::cout << "ioctl failed" << std::endl;
//         }
//         timeSampleTakenX = std::chrono::steady_clock::now();


        
//         conValX = buff[1] + (buff[0] & 0x0f) * 256;
//         chanel = (16 & buff[0]) % 15;
//         print_debug(conValX, chanel);


//         conValX = buff[3] + (buff[2] & 0x0f) * 256;
//         chanel = (16 & buff[2]) % 15;
//         print_debug(conValX, chanel);

//         std::string sampleStr = std::to_string(conValX) + " " +
//                                 std::to_string(conValY);
//         gRawDataForFileWriting.push(sampleStr);
        
//     }
//     return;
// }


/********************** End of file ***************************/



















