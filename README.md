# Graph500 Web Benchmarks

Compares JavaScript vs. WebAssembly performance for four algorithms:

| Algorithm | Author |
|---|---|
| Fibonacci | fibb |
| Strassen's Matrix Multiplication | Alvaro |
| Breadth-First Search (BFS) | Sean |
| Kruskal's MST | Devyani |

Each algorithm has a pure-JS implementation and a C→WASM build (via Emscripten). Fibonacci also has a parallel WASM build using pthreads.

## Setup

Requires the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html):

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh   # or emsdk_env.bat on Windows
```

Also requires `http-server` (any static file server works):

```bash
npm install -g http-server
```

## Build & Run

```bash
# Build all algorithms and serve
make serve

# Build a single algorithm
make build-sean

# Remove build output
make clean
```

Then open `http://localhost:8080` in a browser.
