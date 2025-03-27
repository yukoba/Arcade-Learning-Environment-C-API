# Arcade Learning Environment C API

Forked from [Arcade Learning Environment 0.10.2](https://github.com/Farama-Foundation/Arcade-Learning-Environment/tree/v0.10.2), and the C API was created using Gemini 2.5 Pro.
Tested only on Ubuntu 24.04. See also [Arcade Learning Environment Java API](https://github.com/yukoba/Arcade-Learning-Environment-Java-API).

```sh
sudo apt install libsdl2-dev

mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DSDL_SUPPORT=ON
cmake --build .

cd ../src/ale
cp ../../build/src/ale/libale.so .
gcc ale_test.c -o ale_test -L . -l ale -O3
LD_LIBRARY_PATH=. ./ale_test pong.bin
```
