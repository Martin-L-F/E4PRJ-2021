SOURCES=main.cpp RailSystem_IF.cpp Target_IF.cpp UART/Rx_data.cpp UART/MsgQueue.cpp UART/ScopedLocker.cpp UART/UART.cpp Speaker_IF.cpp
OBJECTS=$(addprefix  $(BUILD_DIR), $(SOURCES:.cpp=.o))
DEPS=$(SOURCES:.cpp=.d)
EXE=ControlUnitApp
CXXFLAGS=-I. -pthread -lboost_system -lcrypto -lssl

# Making for host
# > make ARCH=host
ifeq (${ARCH},host)
CXX=g++
BUILD_DIR=build/host/
EXE_DIR=bin/host/
endif

# Making for target
# > make ARCH=target
ifeq (${ARCH},target)
CXX=arm-rpizw-g++
BUILD_DIR=build/target/
EXE_DIR=bin/target/
endif

$(EXE_DIR)${EXE}: $(DEPS) $(OBJECTS) # << Check the $(DEPS) new dependency
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

# %.cpp -> %.o needs to be added! Target is NOT just %.o...
# Pattern-matching
# No .o files. Build OBJECTS from dependencies one at a time. When all are generated, the executable is built.
${BUILD_DIR}%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
# Rule that describes how a .d (dependency) file is created from a .cpp file
# Similar to the assigment that you just completed %.cpp -> %.o
%.d: %.cpp
	$(CXX) -MT$@ -MM $(CXXFLAGS) $< > $@
	$(CXX) -MT$(@:.d=.o) -MM $(CXXFLAGS) $< >> $@

ifneq ($(filter -out clean format tidy ,$( MAKECMDGOALS )) ,)
-include $(DEPS)
endif


format: $(SOURCES:.cpp=.format)
%.format: %.cpp
	@echo "Formatting file '$<'"...
	@clang-format -i $<
	@echo "" > $@

tidy: $(SOURCES:.cpp=.tidy)
%.tidy: %.cpp
	@echo "Tidying file '$<'"...
	@clang-tidy $< -- $(CXXFLAGS)
	@echo "" > $@

# we'll make our cleaning target more powerful
# we remove the targets that we build by default and also
# remove all object files
.PHONY: clean
clean:	##Clean up your executeable file and all .o and .d files
	rm -f $(EXE) $(BUILD_DIR)*.o $(BUILD_DIR)*.d
	rm -f $(EXE) UART/*.o UART/*.d