
CXX = g++

CXX_FLAGS = -std=c++11 -Wall -Wextra -lwiringPi -lpthread
DEBUG_FLAG = -DDEBUG=1

#Derectory for object and ececutable files.
OBJDIR = ./Executables/

AD7992_DIR = ./AD9772_Comm_Module/

AD9772_MAIN = $(OBJDIR)AD9772_Comm_Main
ERROR_DETECTOR_MAIN = $(OBJDIR)errorDetector

ALL_EXE = $(OBJDIR)AD9772_Comm_Main $(OBJDIR)errorDetector

all: $(OBJDIR)$(AD9772_MAIN) $(OBJDIR)$(ERROR_DETECTOR_MAIN)


$(OBJDIR)$(ERROR_DETECTOR_MAIN): $(OBJDIR)A2D_Comm_I2C.o $(OBJDIR)AD9772_Comm.o $(OBJDIR)errorDetector.o
	$(CXX) $(CXX_FLAGS) $^ -o $(OBJDIR)$(ERROR_DETECTOR_MAIN)

$(OBJDIR)$(AD9772_MAIN): $(OBJDIR)A2D_Comm_I2C.o $(OBJDIR)AD9772_Comm.o  $(AD7992_DIR)AD9772_Comm_Main.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $(OBJDIR)$(AD9772_MAIN)

$(OBJDIR)errorDetector.o: errorDetector.cpp errorDetector.h
	$(CXX) $(CXX_FLAGS) -c $<

$(OBJDIR)AD9772_Comm.o: AD9772_Comm.cpp AD9772_Comm.h 
	$(CXX) $(CXX_FLAGS) -c $<

$(OBJDIR)A2D_Comm_I2C.o: $(AD7992_DIR)A2D_Comm_I2C.cpp $(AD7992_DIR)A2D_Comm_I2C.hpp \
	                     $(AD7992_DIR)A2D_Communication.hpp
	$(CXX) $(CXX_FLAGS) -I $(AD7992_DIR) -c $<

# specify commands that are not files.
.PHONY: clean

#make clean
clean:
	rm -rf $(ALL_EXE) $(OBJDIR)*.o


