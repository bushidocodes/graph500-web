## Simple C Makefile

## Define compiler and flags
CC=emcc
CCFLAGS= -O2 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 

all: build

mkfibb:
	mkdir -p ./src/fibb

clean: 
	rm -fr ./src/fibb

build-wasm: clean mkfibb
	$(CC) ./lib/main.c -o ./src/fibb/fibb.js $(CCFLAGS)

build: build-wasm

serve: build-wasm
	http-server src
