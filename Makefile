cpp-capture: examples/cpp-capture.cpp
	g++ examples/cpp-capture.cpp `pkg-config --cflags --libs glfw3 glu gl` -o rm/cpp-capture
