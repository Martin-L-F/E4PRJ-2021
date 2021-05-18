# define the command that we use for compilation
SOURCES = main.cpp MsgQueue.cpp F_thread.cpp ScopedLocker.cpp
CXXFLAGS= -I. -pthread -Wall
EXE=main_exe
MKDIR=@mkdir -p $(@D)
ARCH=host

#Making for host
#> make ARCH=host
ifeq (${ARCH},host)
CXX = g++
BUILD_DIR=build/host/
BIN_DIR=bin/host/
endif

#Making for target
#> make ARCH=target
ifeq (${ARCH},target)
CXX = arm-rpizw-g++
BUILD_DIR=build/target/
BIN_DIR=bin/target/
endif


OBJECTS=$(addprefix $(BUILD_DIR), $(SOURCES:.cpp=.o))
DEPS=$(addprefix $(BUILD_DIR), $(SOURCES:.cpp=.d))


$(BIN_DIR)$(EXE): $(DEPS) $(OBJECTS)
	$(MKDIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

# %.cpp -> %.o needs to be added! Target is NOT just %.o

# Rule that describes how a .d (dependency) file is created from a .cpp file

# Similar to the assigment that you just completed %.cpp -> %.o

#.PHONY: %

$(BUILD_DIR)%.d: %.cpp
	$(MKDIR)
	$(CXX) -MT$@ -MM $(CXXFLAGS) $< > $@
	$(CXX) -MT$(@:.d=.o) -MM $(CXXFLAGS) $< >> $@

$(BUILD_DIR)%.o: %.cpp
	$(MKDIR)
	$(CXX) -c $(CXXFLAGS) -o $@ $< 


ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

# we'll make our cleaning target more powerful
# we remove the targets that we build by default and also
# remove all object files
.PHONY: clean
clean:	##Clean up your executeable file and all .o and .d files
	rm -f $(EXE) $(BUILD_DIR)*.o $(BUILD_DIR)*.d

.PHONY: help
help: ##Here is your help
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##//'

