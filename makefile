
CXX = g++

CXX_FLAGS = -std=c++11 -Wall -Wextra -lwiringPi -lpthread
DEBUG_FLAG = -DDEBUG=1

AD9772_MAIN = AD9772_Comm_Main
ERROR_DETECTOR_MAIN = errorDetector
ERROR_DETECTOR_DEBUG = errorDetectorDebug

ALL_EXE = AD9772_Comm_Main errorDetector

all: $(AD9772_MAIN) $(ERROR_DETECTOR_MAIN)


$(ERROR_DETECTOR_MAIN): A2D_Comm_I2C.o AD9772_Comm.o  errorDetector.o
	$(CXX) $(CXX_FLAGS) A2D_Comm_I2C.o AD9772_Comm.o errorDetector.o -o $(ERROR_DETECTOR_MAIN)

$(AD9772_MAIN): A2D_Comm_I2C.o AD9772_Comm.o  AD9772_Comm_Main.cpp
	$(CXX) $(CXX_FLAGS) A2D_Comm_I2C.o AD9772_Comm.o AD9772_Comm_Main.cpp \
	-o $(AD9772_MAIN)

errorDetector.o: errorDetector.cpp errorDetector.h
	$(CXX) $(CXX_FLAGS) -c errorDetector.cpp

AD9772_Comm.o: AD9772_Comm.h AD9772_Comm.cpp
	$(CXX) $(CXX_FLAGS) -c AD9772_Comm.cpp

A2D_Comm_I2C.o: A2D_Comm_I2C.cpp A2D_Comm_I2C.hpp A2D_Communication.hpp
	$(CXX) $(CXX_FLAGS) -c A2D_Comm_I2C.cpp

# specify commands that are not files.
.PHONY: clean

#make clean
clean:
	rm -rf $(ALL_EXE) *.o


