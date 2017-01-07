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
    if(ad7992Comm.setAutoSampleMode(Tx32) == MY_ERROR)
    {
        print_error("Set auto mode failed\n");
    }

    if(ad7992Comm.setAddrRegister(CONVERTION_RESULT_REG) == MY_ERROR)
    {
        print_error("Setting addres register failed\n");
    }
    
    WCHAR ch1Val, ch2Val;
    std::chrono::steady_clock::time_point timeBeforeSample;
    std::chrono::steady_clock::time_point timeAfterSample;
    while(TERMINATION_FLAG == false)
    {
        timeBeforeSample = std::chrono::steady_clock::now();
        if(ad7992Comm.readAutoMode(ch1Val, ch2Val, ADDRESS_AS_GND) == MY_ERROR)
        {
            continue;
        }
        else
        {
            timeAfterSample = std::chrono::steady_clock::now();
            auto sampleDuration = timeAfterSample - timeBeforeSample;
            auto microSecs = std::chrono::duration_cast<std::chrono::microseconds>
                                                           (sampleDuration).count();

            /*
            * If the sample took too much time, we had contex switch
            * (thread switch) that makes out time measurments be
            * inaccurate. In this case we ignore the mesurment.
            *
            */
            if(microSecs > MAX_MEASURMENT_TIME_MICRO)
            {
                continue;
            }
            else
            {
                sampleCount++;

                ch1Val = ch1Val & RESULT_MASK;
                ch2Val = ch2Val & RESULT_MASK;
    
                print_debug(ch1Val, 0);
                print_debug(ch2Val, 1);
    
                PSample newSample = new Sample(ch1Val, ch2Val, 
                                               timeBeforeSample);
                gSampleQueue.push(newSample);
    
                std::string sampleStr = std::to_string(ch1Val) + 
                                    " " + std::to_string(ch2Val) +
                                    " " + std::to_string(microSecs);
    
                gRawDataForFileWriting.push(sampleStr);
            }
    
            
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


void AD9772_Manager()
{
    autoSamplingMode();
    //commandModeTwoChan();

    std::cout << "### Thread Done: AD9772_Manager." << std::endl;
    return;
}


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
            ************* DO CALCULATION ON DATA HERE *********.
            *
            * Read the converted data and the time
            * when the sample has been taken from 
            * popedSample.
            *
            * popedSample is the sample to be processed.
            *
            */

            //After finishing working with sample, delete it.
            delete popedSample;
        }
    }
    std::cout << "### Thread Done: dataProcessor." << std::endl;
    return;
}


void dataWriter(const char* fileName)
{
    std::fstream fs;
    std::string popedStr;
    bool popRetVal;
    try
    {
        fs.open(fileName, std::fstream::out);

        //Write the time of thread start in the head of the samples file.
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
    std::cout << "### Thread Done: dataWriter." << std::endl;
    return;
}


void terminateThreads()
{
    //Set the termination flag to true.
    gTerminateFlag.setFlag();
    //Notify all the CV of the thread safe queues.
    gSampleQueue.notifyAll(NULL);

    gRawDataForFileWriting.notifyAll(ZERO_SAMPLE_STR);

}


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


void setSignalHandler()
{
    gSigIntHandler.sa_handler = sigIntHandler;
    sigemptyset(&gSigIntHandler.sa_mask);
    gSigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &gSigIntHandler, NULL);
}


void applicationFunc()
{
    while(TERMINATION_FLAG == false)
    {
        /*
        *
        *
        * Paste application code here
        *
        *
        */
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "### Thread Done:applicationFunc." << std::endl;
}

int main()
{   
    std::cout << std::endl;
    std::cout << "#################################" << std::endl;
    std::cout << "### Welcome to Error Detector ###" << std::endl;
    std::cout << "#################################" << std::endl;
    std::cout << std::endl;

    std::cout << "Current app will run for "
              << std::to_string(TEST_DURATION_SECS) 
              << " seconds." << std::endl;

    std::cout << "You can terminate app by Ctrl+C."
              << std::endl;
    
    const char* fileNameX = "./Samples_From_A2D";

    setSignalHandler();

    std::thread AD9772_ManagerThread(AD9772_Manager);
    std::thread dataProcessorThread(dataProcessor);
    std::thread dataWriterThread(dataWriter,fileNameX);
    std::thread gameAppThread(applicationFunc);

    AD9772_ManagerThread.detach();
    dataProcessorThread.detach();
    dataWriterThread.detach();
    gameAppThread.detach();

    std::this_thread::sleep_for(std::chrono::seconds(TEST_DURATION_SECS));
    sigIntHandler(0);
    return 0;
}

/********************** End of file ***************************/



















