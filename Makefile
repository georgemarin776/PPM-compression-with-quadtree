all: build run

build: 
	gcc main.c -g -o quadtree -lm

run: 
	./quadtree -c 0 Imagini/test4.ppm out.ppm