# C-CNC

This is the repository of the code developed during the Manufacturing Automation
course, Department of Industrial Engineering, University of Trento.

## Contents

* `examples`: contains introductory code examples
* `src`: contains the main project source code (library files)
*  `src/main`: contains executables code

## Build with Cmake

Building a project with Cmake is a two-step process. The first step is called
*configuring*, and results in populating the `build` folder with all the
contents needed for the compilation. The second step is called *compilation* and
results in the products of the build to be created in the root of the `build`
folder. There is an optional third step, *install*, that copies the build
products into a destination folder. This project os configured to have the local `bin` forder as destination.

1. (configuring) from the terminal, be sure to be in the project's toot directory and then issue the command `cmake
   -Bbuild -H.`: this means configure the project in the `build` directory, searching
   for the `CMakeLists.txt` file in the current directory (taht is `.`)
3. (compilation) from the terminal, compile the project with the command `make -Cbuild` 
4. (optional install) if you want to install the build products, type `make - Cbuild install`: this copies binaries into the `bin` and `lib` folders of the root
   project folder

**Note**: the `cmake` command must be run the first time, and then every time
that you create, move, or rename source files. Conversely, if you only change
contents of source files, then you only need to `make`. The `make` command is
smart enough not to recompile files that have been already compiled and that are
unchanged from the previous build: this reduces a lot the compilation time for
large projects.

**Note**: the command `make install` also does the compilation, so if you want
the products in the install folder just call `make install` (i.e. there is no
need for calling `make` and then `make install`)

## Prerequisites

If you want to use your own custom linux (Ubuntu or Debian), be sure to have the
following packages:

```sh
sudo apt install build-essential cmake clang lldb lua5.3 libglu1-mesa-dev freeglut3-dev mesa-common-dev libreadline-dev
```