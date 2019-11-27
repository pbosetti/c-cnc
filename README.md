# C-CNC 2019-20

This is the repository of the code developed during the Manufacturing Automation course, academic year 2019-20, University od Trento.

## Contents

* `examples`: contains introductory code examples
* `src`: contains the main project source code (library files)
*  `src/main`: contains executables code

## Build with Cmake

Building a project with Cmake is a two-step process. The first step is called *configuring*, and results in populating the `build` folder with all the contents needed for the compilation. The second step is called *compilation* and results in the products of the build to be created in the root of the `build` folder. There is an optional third step, *install*, that copies the build products into a destination folder.

1. (preliminary) if it does not exist already, create the folder `build` just below the root of your project folder
2. (configuring) from the terminal, `cd build` and the issue the command `cmake ..`: this means *configure the project in the current directory, searching for the `CMakeLists.txt` file in the previous directory
3. (compilation) from the terminal, compile the project with the command `make` 
4. (optional install) if you want to install the build products, type `make install`: this copies binaries into the `bin` and `lib` folders of the root project folder

**Note**: the `cmake` command must be run the first time, and then every time that you create, move, or rename source files. Conversely, if you only change contents of source files, then you only need to `make`. The `make` command is smart enough not to recompile files that have been already compiled and that are unchanged from the previous build: this reduces a lot the compilation time for large projects.

**Note**: the command `make install` also does the compilation, so if you want the products in the install folder just call `make install` (i.e. there is no need for calling `make` and then `make install`)