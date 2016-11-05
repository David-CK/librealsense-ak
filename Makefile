CXXFLAGS := -std=c++11 -fPIC

OBJECTS = $(notdir $(basename $(wildcard src/*.cpp)))
OBJECTS := $(addprefix obj/, $(addsuffix .o, $(OBJECTS)))

EXAMPLES := $(wildcard examples/*.c)
EXAMPLES += $(wildcard examples/*.cpp)
EXAMPLES := $(addprefix bin/, $(notdir $(basename $(EXAMPLES))))

# Aliases for convenience
all: examples $(EXAMPLES)

#install:

#uninstall:

clean:
	rm -rf obj
	rm -rf lib
	rm -rf bin

obj lib bin:
	mkdir obj
	mkdir lib
	mkdir bin
exe:
	LD_LIBRARY_PATH=lib/
	./bin/c-tutorial-1-depth


bin/c-%: examples/c-%.c lib/librealsense.so | bin
	$(CC) $< -Iinclude -Llib -lrealsense -o $@

bin/cpp-%: examples/cpp-%.cpp lib/librealsense.so | bin
	$(CXX) $< -Iinclude -Llib -lrealsense -o $@

lib/librealsense.so: $(OBJECTS) | lib
#-I
#	#$(CXX) -Iinclude -shared $(OBJECTS) -o $@
	$(CXX) -shared $(OBJECTS) -o $@

obj/%.o: src/%.cpp | obj
#-I
	$(CXX) $< -Iinclude $(CXXFLAGS) -c -o $@
#	#$(CXX) $< $(CXXFLAGS) -c -o $@

#LD_LIBRARY_PATH=lib ./bin/c-tutorial-1-depth
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
#cpp-capture: examples/cpp-capture.cpp
#	g++ examples/cpp-capture.cpp `pkg-config --cflags --libs glfw3 glu gl` -o rm/cpp-capture

