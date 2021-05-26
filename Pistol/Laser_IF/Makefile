CXX = arm-rpizw-g++
SOURCES = main.cpp Laser_IF.cpp
OBJECTS = $(SOURCES:.cpp=.o)
DEPS = $(SOURCES:.cpp=.d)
EXE = test_laser_class_v1
CXXFLAGS = -I.

$(EXE): $(DEPS) $(OBJECTS) # << Check the $(DEPS) new dependency
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

# Rule that describes how a .d (dependency) file is created from a .cpp file
# Similar to the assigment that you just completed %.cpp -> %.o

%.d: %.cpp
	$(CXX) -MT$@ -MM $(CXXFLAGS) $< > $@
	$(CXX) -MT$(@:.d=.o) -MM $(CXXFLAGS) $< >> $@

-include $(DEPS)

.PHONY: clean
clean: ##Clean up you executable file and all .o files
	rm -f $(EXE) *.o *.d