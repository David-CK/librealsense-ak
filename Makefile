CXXFLAGS := -std=c++11

OBJECTS = $(notdir $(basename $(wildcard src/*.cpp)))
OBJECTS := $(addprefix obj/, $(addsuffix .o, $(OBJECTS)))

EXAMPLES := $(wildcard examples/*.c)
EXAMPLES += $(wildcard examples/*.cpp)
EXAMPLES := $(addprefix bin/, $(notdir $(basename $(EXAMPLES))))

# Aliases for convenience
all: examples $(EXAMPLES)

install:

uninstall:

clean:
	rm -rf obj
	rm -rf lib
	rm -rf bin

obj lib bin:
	mkdir obj
	mkdir lib
	mkdir bin


bin/c-%: examples/c-%.c lib/librealsense.so | bin
	$(CC) $< -o $@

bin/cpp-%: examples/cpp-%.cpp lib/librealsense.so | bin
	$(CXX) $< -o $@

lib/librealsense.so: $(OBJECTS) | lib
	$(CXX) -shared $(OBJECTS) -o $@

obj/%.o: src/%.cpp | obj
	$(CXX) $< $(CXXFLAGS) -c -o $@

cpp-capture: examples/cpp-capture.cpp
	g++ examples/cpp-capture.cpp `pkg-config --cflags --libs glfw3 glu gl` -o rm/cpp-capture

