build:
	mkdir -p build
	gcc -o build/pre4k pre4k.c
run:
	PATH=$PATH:./build pre4k
