triangle: src/*.c src/*.cpp
	g++ -g -I/home/jay/open_source/glfw/include -I./include/ src/*.c src/*.cpp /home/jay/open_source/glfw/build/src/libglfw3.a -o triangle

run: triangle
	./triangle

