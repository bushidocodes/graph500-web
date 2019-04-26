## Simple C Makefile

## Define compiler and flags
CC=emcc
CCFLAGSBASE= -O0 -g \
	-s STRICT=1 \
	-s MALLOC=dlmalloc \
	-s WASM=1 
CCFLAGS= ${CCFLAGSBASE} \
	-s SIDE_MODULE=1 
CCFLAGSBFS= ${CCFLAGSBASE} \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s ASSERTIONS=1  \
	-s EXPORTED_FUNCTIONS='["_createGraph", "_insertEdge", "_runBFS", "_getParent"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-s SAFE_HEAP=1 #\
	-s TOTAL_MEMORY=1999962112
CCFLAGSKRUSKAL= ${CCFLAGSBASE} \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s ASSERTIONS=1  \
	-s EXPORTED_FUNCTIONS='["_init", "_insertadjver", "_kruskal", "_printResults"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-s SAFE_HEAP=1
CCFLAGSSTRASSENS= ${CCFLAGSBASE} \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s ASSERTIONS=1  \
	-s EXPORTED_FUNCTIONS='["_matrixMultiplication"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-s SAFE_HEAP=1
CCFLAGSPTHREADS= ${CCFLAGSBASE} \
	-s USE_PTHREADS=1 \
	-s PTHREAD_POOL_SIZE=4 


all: build

make-dist:
	mkdir -p ./dist/resources

build-deps: make-dist
	cp -r ./src/resources ./dist
	cp ./src/htmlTemplates/index.html ./dist/index.html
	cp -r ./src/libs ./dist

clean-fibb: 
	rm -rf ./dist/fibb

make-fibb: clean-fibb
	mkdir -p ./dist/fibb
	mkdir -p ./dist/fibb/wasm
	mkdir -p ./dist/fibb/wasm-pthread
	mkdir -p ./dist/fibb/js

build-fibb: build-deps clean-fibb make-fibb
	$(CC) ./src/fibb/wasm/main.c -o ./dist/fibb/wasm/main.wasm $(CCFLAGS)
	$(CC) ./src/fibb/wasm-pthread/main.c -o ./dist/fibb/wasm-pthread/main.js $(CCFLAGSPTHREADS)
	cp ./src/fibb/js/main.js ./dist/fibb/js/main.js
	cp ./src/fibb/wasm/main.js ./dist/fibb/wasm/main.js
	cp ./src/htmlTemplates/indexChild.html ./dist/fibb/wasm/index.html
	cp ./src/htmlTemplates/indexPthreads.html ./dist/fibb/wasm-pthread/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/fibb/js/index.html
	cp -r ./src/fibb/common ./dist/fibb/

serve-fibb: build-fibb
	http-server dist

clean-alvaro: 
	rm -rf ./dist/alvaro

make-alvaro: 
	mkdir -p ./dist/alvaro
	mkdir -p ./dist/alvaro/wasm
	mkdir -p ./dist/alvaro/wasm-pthread
	mkdir -p ./dist/alvaro/js

build-alvaro: build-deps clean-alvaro make-alvaro
	$(CC) ./src/alvaro/wasm/main.c -o ./dist/alvaro/wasm/emscripten.js $(CCFLAGSSTRASSENS)
	# $(CC) ./src/alvaro/wasm-pthread/main.c -o ./dist/alvaro/wasm-pthread/main.js $(CCFLAGSPTHREADS)
	cp -r ./src/alvaro/js ./dist/alvaro
	cp ./src/alvaro/wasm/main.js ./dist/alvaro/wasm/main.js
	cp ./src/htmlTemplates/indexGlueCode.html ./dist/alvaro/wasm/index.html
	cp ./src/htmlTemplates/indexPthreads.html ./dist/alvaro/wasm-pthread/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/alvaro/js/index.html
	cp -r ./src/alvaro/common ./dist/alvaro/

serve-alvaro: build-alvaro
	http-server dist

clean-sean: 
	rm -rf ./dist/sean

make-sean: 
	mkdir -p ./dist/sean
	mkdir -p ./dist/sean/wasm
	mkdir -p ./dist/sean/wasm-pthread
	mkdir -p ./dist/sean/js

build-sean: build-deps clean-sean make-sean
	$(CC) ./src/sean/wasm/main.c -o ./dist/sean/wasm/emscripten.js $(CCFLAGSBFS) 
	# $(CC) ./src/sean/wasm-pthread/main.c -o ./dist/sean/wasm-pthread/main.js $(CCFLAGSPTHREADS) 
	cp -r ./src/sean/js ./dist/sean
	cp ./src/sean/wasm/main.js ./dist/sean/wasm/main.js
	cp ./src/htmlTemplates/indexGlueCode.html ./dist/sean/wasm/index.html
	cp ./src/htmlTemplates/indexPthreads.html ./dist/sean/wasm-pthread/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/sean/js/index.html
	cp -r ./src/sean/common ./dist/sean/

serve-sean: build-sean
	http-server dist

clean-devyani: 
	rm -rf ./dist/devyani

make-devyani: 
	mkdir -p ./dist/devyani
	mkdir -p ./dist/devyani/wasm
	mkdir -p ./dist/devyani/wasm-pthread
	mkdir -p ./dist/devyani/js

build-devyani: build-deps clean-devyani make-devyani
	$(CC) ./src/devyani/wasm/main.c -o ./dist/devyani/wasm/emscripten.js $(CCFLAGSKRUSKAL)
	# $(CC) ./src/devyani/wasm-pthread/main.c -o ./dist/devyani/wasm-pthread/main.js $(CCFLAGSPTHREADS)
	cp -r ./src/devyani/js ./dist/devyani
	cp ./src/devyani/wasm/main.js ./dist/devyani/wasm/main.js
	cp ./src/htmlTemplates/indexGlueCode.html ./dist/devyani/wasm/index.html
	cp ./src/htmlTemplates/indexPthreads.html ./dist/devyani/wasm-pthread/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/devyani/js/index.html
	cp -r ./src/devyani/common ./dist/devyani/

serve-devyani: build-devyani
	http-server dist

build: build-fibb build-alvaro build-sean build-devyani

serve: build
	http-server dist
