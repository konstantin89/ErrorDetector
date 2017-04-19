#######################################################
#
# Author: Kosta Bulgakov
#
# Brief: Make file for Error detector app.
#
#######################################################


CXX = g++

CXX_FLAGS = -std=c++11 -Wall -Wextra -lwiringPi -lpthread
DEBUG_FLAG = -DDEBUG=1

#Derectory for object and ececutable files.
OBJDIR = ./ObjsAndExecs/
AD7992_DIR = ./AD9772_Comm_Module/
ERR_DET_DIR = ./Error_Detector/

#Text files created by application.
PARAM_FILES = ParametersTest Parameters
LOG_FILES = LogFile LogFileTest

#Commands
AD9772_MAIN = $(OBJDIR)AD9772_Comm_Main
ERROR_DETECTOR_MAIN = $(OBJDIR)errorDetector
LOG_MODULE_TEST_MAIN = $(OBJDIR)logModuleTest
JOYSTICK_CALIBRATION = $(OBJDIR)joysticCalibration




all: $(AD9772_MAIN) $(ERROR_DETECTOR_MAIN) $(LOG_MODULE_TEST_MAIN)\
	                $(JOYSTICK_CALIBRATION)


########################## Executables ###############################

$(ERROR_DETECTOR_MAIN): $(OBJDIR)A2D_Comm_I2C.o $(OBJDIR)AD9772_Comm.o \
		        $(OBJDIR)errorDetector.o $(OBJDIR)LogModule.o \
		        $(ERR_DET_DIR)Sample.h
	$(CXX) $(CXX_FLAGS) $^ -o $@


$(AD9772_MAIN): $(OBJDIR)A2D_Comm_I2C.o $(OBJDIR)AD9772_Comm.o \
                $(AD7992_DIR)AD9772_Comm_Main.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@


$(JOYSTICK_CALIBRATION): $(ERR_DET_DIR)joysticCalibration.cpp\
	                     $(OBJDIR)AD9772_Comm.o \
	                     $(OBJDIR)A2D_Comm_I2C.o
	$(CXX) $(CXX_FLAGS)  $^ -o $@


$(LOG_MODULE_TEST_MAIN): $(ERR_DET_DIR)LogModule.cpp      \
	                     $(ERR_DET_DIR)LogModule.h        \
	                     $(ERR_DET_DIR)logModuleTests.cpp \
	                     $(ERR_DET_DIR)TestParameters.h   \
	                     $(ERR_DET_DIR)ThreadSafeQueue.hpp
	$(CXX) $(CXX_FLAGS)  $^ -o $@


############################## Object Files ###########################


$(OBJDIR)errorDetector.o: $(ERR_DET_DIR)errorDetector.cpp \
	                      $(ERR_DET_DIR)errorDetector.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJDIR)LogModule.o: $(ERR_DET_DIR)LogModule.cpp $(ERR_DET_DIR)LogModule.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJDIR)AD9772_Comm.o: $(AD7992_DIR)AD9772_Comm.cpp \
	                    $(AD7992_DIR)AD9772_Comm.h 
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJDIR)A2D_Comm_I2C.o: $(AD7992_DIR)A2D_Comm_I2C.cpp \
	                     $(AD7992_DIR)A2D_Comm_I2C.hpp \
	                     $(AD7992_DIR)A2D_Communication.hpp
	$(CXX) $(CXX_FLAGS)  -c $< -o $@


######################## Other Commands ###########################

# specify commands that are not files.
.PHONY: clean

#make clean
clean:
	rm -rf $(OBJDIR)* $(LOG_FILES) $(PARAM_FILES)




######################## End Of File #############################