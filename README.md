## About

![raycaster-img](https://github.com/user-attachments/assets/7e0b2bef-6a90-47e8-9995-6923f45ef297)

A simple Wolfenstein 3D style software renderer raycaster game demo made in C with SDL2.

## Build

Make sure you have CMake installed ([https://cmake.org/download/](https://cmake.org/download/)) as well as your C compiler of choice.

After cloning this repository, create a new directory to build the project in:

```
mkdir build
```

Use CMake to generate the build files:

```
cmake -B ./build -S . -DCMAKE_BUILD_TYPE="Release"
```

Next, build the project:

```
cmake --build ./build
```

To run the project:

```
./build/raycaster.exe
```