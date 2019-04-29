# WASM Fibb

## Setup
This app requires the Emscripten toolchain, which can be installed using the folllowing commands on Mac or Linux:

```bash
cd ~
mkdir Tooling
cd Tooling
git clone https://github.com/juj/emsdk.git
cd emsdk
./emsdk install sdk-1.38.15-64bit
./emsdk activate sdk-1.38.15-64bit
```

## Running the App
```bash
make serve
```
