# Linerenderer

A simple 3d line renderer based off of my work for the [Ubisoft Next 2023](https://github.com/StewartChandler/UbisoftNext2023) competition.  During the competition, I only started halfway through the second day, and didn't have as much time as I would have liked.  I also felt that trying to implement a template-based linear algebra library put me severely over my head and caused a lot of trouble for me later down the line.  I didn't get to implement some of the features that I would have liked to, so this project is a way of getting to explore the ideas that I didn't get a chance to before.

## Build

The cmake script provided uses vcpkg to manage and link libraries, and has only been tested on x86_64 windows.

### Installing dependencies

First, you should have [vcpkg](https://github.com/microsoft/vcpkg) installed on your system, and make sure you have your `VCPKG_ROOT` environment variable set correctly.

Then, using vcpkg, install the necessary dependencies. 
```sh
vcpkg install glfw3:x64-windows
vcpkg install gl3w:x64-windows
vcpkg install glm:x64-windows
```
### Generating and building files with cmake
With the dependencies installed, navigate to the root directory of the repository, and invoke cmake. I prefer to create a separate directory for the build generated by cmake, so the following commands reflect that.
```sh
mkdir build
cmake -B ./build
```
Then, to actually build the project run the following:
```sh
cmake --build ./build
```
### Building in Release mode
This changes depending on which cmake generator it uses from what I have read and tried the following commands are what should be used.  I used Visual Studio as my generator, which is a multi-config generator, so I use the latter, for more information see [this link](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations).
#### Using a single-config generator
Instead configure cmake with the following command:
```sh
cmake -B ./build -DCMAKE_BUILD_TYPE=Release
```
#### Using a multi-config generator
Instead build with the following command:
```sh
cmake --build ./build --config Release
```
### Running the built files
Then to run the resulting binary, this next step will depend on what generator you used for cmake, I used Visual Studio, so the resulting binary is built with the path `build/Debug/linerenderer.exe`. So to run it, we have the following:
```sh
./build/Debug/linerenderer.exe
```
Or in the case of the Release build (or any other type of build):
```sh
./build/Release/linerenderer.exe
```