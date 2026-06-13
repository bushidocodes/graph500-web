## Simple C Makefile

## Define compiler and flags
CC=emcc
TESTCFLAGS= -Wall -Wextra
CCFLAGSBASE= -O3 \
	-Wall \
	-Wextra \
	-s STRICT=1 \
	-s MALLOC=dlmalloc \
	-s WASM=1 
CCFLAGSFIBB= ${CCFLAGSBASE} \
	-s EXPORTED_FUNCTIONS='["_fibonacci"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
CCFLAGSBFS= ${CCFLAGSBASE} \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s EXPORTED_FUNCTIONS='["_createGraph", "_insertEdge", "_runBFS", "_getParent", "_compressData"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-s SAFE_HEAP=1 #\
	-s ASSERTIONS=1  \
	-s TOTAL_MEMORY=1999962112
CCFLAGSKRUSKAL= ${CCFLAGSBASE} \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s EXPORTED_FUNCTIONS='["_init", "_insertadjver", "_kruskal", "_printResults"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-s SAFE_HEAP=1
	# -s ASSERTIONS=1  
CCFLAGSSTRASSENS= ${CCFLAGSBASE} \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s EXPORTED_FUNCTIONS='["_matrixMultiplication"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-s SAFE_HEAP=1
	# -s ASSERTIONS=1  
CCFLAGSPTHREADS= ${CCFLAGSBASE} \
	-pthread \
	-s PTHREAD_POOL_SIZE=4


all: build

make-dist:
	mkdir -p ./dist/resources

build-deps: make-dist
	cp -r ./src/resources ./dist
	cp ./src/htmlTemplates/index.html ./dist/index.html
	@if [ -d ./src/libs ]; then cp -r ./src/libs ./dist; fi

clean-fibb: 
	rm -rf ./dist/fibb

make-fibb: clean-fibb
	mkdir -p ./dist/fibb
	mkdir -p ./dist/fibb/wasm
	mkdir -p ./dist/fibb/wasm-pthread
	mkdir -p ./dist/fibb/js

build-fibb: build-deps clean-fibb make-fibb
	$(CC) ./src/fibb/wasm/main.c -o ./dist/fibb/wasm/emscripten.js $(CCFLAGSFIBB)
	$(CC) ./src/fibb/wasm-pthread/main.c -o ./dist/fibb/wasm-pthread/main.js $(CCFLAGSPTHREADS)
	cp ./src/fibb/js/main.js ./dist/fibb/js/main.js
	cp ./src/fibb/wasm/main.js ./dist/fibb/wasm/main.js
	cp ./src/htmlTemplates/indexGlueCode.html ./dist/fibb/wasm/index.html
	cp ./src/htmlTemplates/indexPthreads.html ./dist/fibb/wasm-pthread/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/fibb/js/index.html
	cp -r ./src/fibb/common ./dist/fibb/

serve-fibb: build-fibb
	http-server dist

clean-alvaro: 
	rm -rf ./dist/alvaro

make-alvaro:
	mkdir -p ./dist/alvaro/wasm
	mkdir -p ./dist/alvaro/js

build-alvaro: build-deps clean-alvaro make-alvaro
	$(CC) ./src/alvaro/wasm/main.c -o ./dist/alvaro/wasm/emscripten.js $(CCFLAGSSTRASSENS)
	cp -r ./src/alvaro/js ./dist/alvaro
	cp ./src/alvaro/wasm/main.js ./dist/alvaro/wasm/main.js
	cp ./src/htmlTemplates/indexGlueCode.html ./dist/alvaro/wasm/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/alvaro/js/index.html
	cp -r ./src/alvaro/common ./dist/alvaro/

serve-alvaro: build-alvaro
	http-server dist

clean-sean: 
	rm -rf ./dist/sean

make-sean:
	mkdir -p ./dist/sean/wasm
	mkdir -p ./dist/sean/js

build-sean: build-deps clean-sean make-sean
	$(CC) ./src/sean/wasm/main.c -o ./dist/sean/wasm/emscripten.js $(CCFLAGSBFS)
	cp -r ./src/sean/js ./dist/sean
	cp ./src/sean/wasm/main.js ./dist/sean/wasm/main.js
	cp ./src/htmlTemplates/indexGlueCode.html ./dist/sean/wasm/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/sean/js/index.html
	cp -r ./src/sean/common ./dist/sean/

serve-sean: build-sean
	http-server dist

clean-devyani: 
	rm -rf ./dist/devyani

make-devyani:
	mkdir -p ./dist/devyani/wasm
	mkdir -p ./dist/devyani/js

build-devyani: build-deps clean-devyani make-devyani
	$(CC) ./src/devyani/wasm/main.c -o ./dist/devyani/wasm/emscripten.js $(CCFLAGSKRUSKAL)
	cp -r ./src/devyani/js ./dist/devyani
	cp ./src/devyani/wasm/main.js ./dist/devyani/wasm/main.js
	cp ./src/htmlTemplates/indexGlueCode.html ./dist/devyani/wasm/index.html
	cp ./src/htmlTemplates/indexChild.html ./dist/devyani/js/index.html
	cp -r ./src/devyani/common ./dist/devyani/

serve-devyani: build-devyani
	http-server dist

build: build-fibb build-alvaro build-sean build-devyani

clean:
	rm -rf ./dist

serve: build
	http-server dist

test-sean:
	mkdir -p ./dist/tests
	gcc $(TESTCFLAGS) -I./src/sean/tests/unity \
		./src/sean/tests/unity/unity.c \
		./src/sean/tests/test_graph.c \
		-o ./dist/tests/test_sean.exe
	./dist/tests/test_sean.exe

test-bfs:
	mkdir -p ./dist/tests
	gcc $(TESTCFLAGS) -I./src/sean/tests/unity \
		./src/sean/tests/unity/unity.c \
		./src/sean/tests/test_bfs.c \
		-o ./dist/tests/test_bfs.exe
	./dist/tests/test_bfs.exe

test-kruskal:
	mkdir -p ./dist/tests
	gcc $(TESTCFLAGS) -I./src/sean/tests/unity \
		./src/sean/tests/unity/unity.c \
		./src/devyani/tests/test_kruskal.c \
		-o ./dist/tests/test_kruskal.exe
	./dist/tests/test_kruskal.exe

test-strassen:
	mkdir -p ./dist/tests
	gcc $(TESTCFLAGS) -I./src/sean/tests/unity \
		./src/sean/tests/unity/unity.c \
		./src/alvaro/tests/test_strassen.c \
		-o ./dist/tests/test_strassen.exe
	./dist/tests/test_strassen.exe

test-malloc-guards:
	mkdir -p ./dist/tests
	gcc $(TESTCFLAGS) -I./src/sean/tests/unity \
		./src/sean/tests/unity/unity.c \
		./src/sean/tests/test_malloc_guards.c \
		-o ./dist/tests/test_malloc_guards.exe
	./dist/tests/test_malloc_guards.exe
