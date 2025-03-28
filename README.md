# Arcade Learning Environment C API

Forked from [Arcade Learning Environment 0.10.2](https://github.com/Farama-Foundation/Arcade-Learning-Environment/tree/v0.10.2), and the C API was created using Gemini 2.5 Pro.
Tested on Ubuntu 24.04 and MSYS2. See also [Arcade Learning Environment Java API](https://github.com/yukoba/Arcade-Learning-Environment-Java-API).

## Ubuntu 24.04

```sh
sudo apt install cmake libsdl2-dev

mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DSDL_SUPPORT=ON
cmake --build .

cd ../src/ale
cp ../../build/src/ale/libale.so .
gcc ale_test.c -o ale_test -L . -l ale -O3
LD_LIBRARY_PATH=. ./ale_test pong.bin
```

## MSYS2 MINGW64 (Windows)

```sh
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-dlfcn mingw-w64-x86_64-libltdl mingw-w64-x86_64-SDL2

mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DSDL_SUPPORT=ON
cmake --build .

cd ../src/ale
cp ../../build/src/ale/libale.dll .
gcc ale_test.c -o ale_test -L . -l ale -O3
./ale_test.exe pong.bin
```
