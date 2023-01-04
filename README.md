# Messing around with opengl

Little playground for writing some C and drawing in 3D. It should be reasonably cross platform compatible. Haven't tried compiling on osx or linux yet, but should be able to tweak the CMake configuration to support whatever.

## System Library Requirements

* some reasonably standard C17 compatible toolchain/environment
* cmake
* OpenGL like 3,4-ish?
* math.h implementation/libm

## Included Third-Party Libraries

* [GLAD2](https://gen.glad.sh/): Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator.
* [GLFW](https://www.glfw.org): (**included as a git submodule**) Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop. It provides a simple API for creating windows, contexts and surfaces, receiving input and events.
* [linmath](https://github.com/datenwolf/linmath.h): A small library for linear math as required for computer graphics
* [json.h](https://github.com/sheredom/json.h): A simple single header solution to parsing JSON in C and C++.
* [utest.h](https://github.com/sheredom/utest.h): A simple one header solution to unit testing for C/C++.
* [stb_image.h](https://github.com/nothings/stb): image loader single-file public domain (or MIT licensed) libraries for C/C++

General Place to grab small reusable things: https://github.com/nothings/single_file_libs
Source of OpenGL and other graphics examples: https://learnopengl.com

## Getting Started

```
git clone git@github.com:ebittleman/mingl.git
git submodule init
git submodule update --recursive
```

## To Configure and Build

```
cmake -S . -B ./build && cmake --build ./build
```

(can also just hit F7 in vscode to build)

## To Configure Build and Run

```
cmake -S . -B ./build && cmake --build ./build && ./build/src/mingl.exe
```

## Debugger

Use whatever you normally do. I've been using `lldb` with the [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) VSCode extension. I included my `launch.json` in the `.vscode` directory. Just hitting `F5` should get it all going. Breakpoints set in the gutters of VSCode and all the other bells and whistles seem to work pretty well. Just need to make sure you set the debug target with the VSCode command `CMake: Set Debug Target` to switch between the main application and tests.

## My Setup

* Windows 10
* Clang installed via MSYS2 (https://www.msys2.org/)
    * `pacman -S mingw-w64-clang-x86_64-toolchain`
    * `pacman -S mingw-w64-clang-x86_64-cmake`
    * Add `C:\msys64\clang64\bin` to system path
* VSCode
    * [C/C++ for Visual Studio Code](https://code.visualstudio.com/docs/languages/cpp)
    * [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
    * [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb): A native debugger powered by LLDB. Debug C++, Rust and other compiled languages.
    * I committed the `.vscode` directory with settings that should work on all systems (no absolute paths specific to my system)
