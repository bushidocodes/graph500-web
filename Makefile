## Simple C Makefile

## Define compiler and flags
CC=emcc
CCFLAGS= -O2 
CCFLAGSPTHREADS= ${CCFLAGS} -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 

all: build

mkfibb:
	mkdir -p ./dist
	mkdir -p ./dist/fibb
	mkdir -p ./dist/fibb/wasm
	mkdir -p ./dist/fibb/wasm-pthread
	mkdir -p ./dist/fibb/js
	mkdir -p ./dist/resources

clean: 
	rm -rf ./dist

build-wasm: clean mkfibb
	$(CC) ./src/fibb/wasm/main.c -o ./dist/fibb/wasm/main.js $(CCFLAGS)
	$(CC) ./src/fibb/wasm-pthread/main.c -o ./dist/fibb/wasm-pthread/main.js $(CCFLAGSPTHREADS)

copy-js: mkfibb
	cp ./src/fibb/js/main.js ./dist/fibb/js/main.js

copy-html: mkfibb
	cp ./src/htmlTemplates/index.html ./dist/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/fibb/wasm/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/fibb/wasm-pthread/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/fibb/js/index.html

copy-resources: mkfibb
	cp -r ./src/resources ./dist

build: build-wasm copy-html copy-js copy-resources

serve: build
	http-server dist
