#Push changes to github.
#git push -u origin master



CXX = g++

CXX_FLAGS = -std=c++11 -Wall -Wextra -lwiringPi -lpthread
DEBUG_FLAG = -DDEBUG=1

#Derectory for object and ececutable files.
OBJDIR = ./ObjsAndExecs/
AD7992_DIR = ./AD9772_Comm_Module/
ERR_DET_DIR = ./Error_Detector/

AD9772_MAIN = $(OBJDIR)AD9772_Comm_Main
ERROR_DETECTOR_MAIN = $(OBJDIR)errorDetector


all: $(AD9772_MAIN) $(ERROR_DETECTOR_MAIN)


$(ERROR_DETECTOR_MAIN): $(OBJDIR)A2D_Comm_I2C.o $(OBJDIR)AD9772_Comm.o $(OBJDIR)errorDetector.o
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(AD9772_MAIN): $(OBJDIR)A2D_Comm_I2C.o $(OBJDIR)AD9772_Comm.o  $(AD7992_DIR)AD9772_Comm_Main.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(OBJDIR)errorDetector.o: $(ERR_DET_DIR)errorDetector.cpp $(ERR_DET_DIR)errorDetector.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJDIR)AD9772_Comm.o: $(AD7992_DIR)AD9772_Comm.cpp $(AD7992_DIR)AD9772_Comm.h 
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJDIR)A2D_Comm_I2C.o: $(AD7992_DIR)A2D_Comm_I2C.cpp $(AD7992_DIR)A2D_Comm_I2C.hpp \
	                     $(AD7992_DIR)A2D_Communication.hpp
	$(CXX) $(CXX_FLAGS)  -c $< -o $@

# specify commands that are not files.
.PHONY: clean

#make clean
clean:
	rm -rf $(OBJDIR)*


