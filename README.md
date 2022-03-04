# C-CNC

This is the repository of the code developed during the _Digital Manufacturing_
course, Department of Industrial Engineering, University of Trento.

## Contents

* `examples`: contains introductory code examples
* `src`: contains the main project source code (library files)
* `src/main`: contains executables code
* `goodies`: useful stuff
* `products_host`: destination folder for compilet binaries
* `products`: destination folder for cross-compiled binaries

## VS Code setup

The development is carried out in Visual Studio Code (VS Code for brevity).

I suggest to congugure VS Code with the following settings. Open the settings file: `Ctrl`+`Shift`+`p` then type `json` and select the item "Preferences: Open Settings (JSON)". Then be sure that the list contains the following items:

```json
{
  "editor.fontFamily": "Fira Code",
  "editor.fontLigatures": true,
  "editor.tabSize": 2,
  "editor.insertSpaces": true,
  "editor.wrappingIndent": "indent",
  "editor.renderControlCharacters": true,
  "editor.wordWrap": "bounded",
  "editor.wordWrapColumn": 80,
  "editor.rulers": [80],
  "editor.renderLineHighlight": "all",
}
```

## Prerequisites

The project must be built with a linux toolchain. On Windows, we are using a WSL2 environment with Ubuntu OS. To enable the compilation we need to install a few packages: on the linux console, type:

```bash
sudo apt install build-essential make cmake cmake-curses-gui clang clang-format lldb libzmq3-dev ruby figlet
sudo gem install gv_fsm
code --install-extension xaver.clang-format
code --install-extension tintinweb.graphviz-interactive-preview
code --install-extension canna.figlet
sudo update-alternatives --set c++ /usr/bin/clang++
sudo update-alternatives --set cc /usr/bin/clang
```


## Build with Cmake

Building a project with Cmake is a two-step process. The first step is called
*configuring*, and results in populating the `build` folder with all the
contents needed for the compilation. The second step is called *compilation* and
results in the products of the build to be created in the root of the `build`
folder. There is an optional third step, *install*, that copies the build
products into a destination folder. This project os configured to have the local `bin` forder as destination.

1. (configuring) from the terminal, be sure to be in the project's toot directory and then issue the command `cmake
	-Bbuild .`: this means configure the project in the `build` directory, searching
	for the `CMakeLists.txt` file in the current directory (that is `.`)
3. (compilation) from the terminal, compile the project with the command `make -C build` 
4. (optional install) if you want to install the build products, type `make -C build install`: this copies binaries into the `bin` and `lib` folders of the root
	project folder

**Note**: the `cmake` command must be run the first time, and then every time
that you create, move, or rename source files. Conversely, if you only change
contents of source files, then you only need to `make`. The `make` command is
smart enough not to recompile files that have been already compiled and that are
unchanged from the previous build: this reduces a lot the compilation time for
large projects.

**Note**: the command `make install` also does the compilation **if needed**, so if you want
the products in the install folder just call `make install` (i.e. there is no
need for calling `make` and then `make install`)

For brevity sake, after having configured the project for the first time, in the following you can do everithing with one single command: `cmake --build build -t install`: this is doing, in sequence, step 1 (only if the CMakeLists.txt file has changed), then step 2 (only if sources have changed), then step 3.

## Other projects

The cross-build system is taken from <https://github.com/pbosetti/xtemplate>.

## Author

Paolo Bosetti (paolo dot bosetti at unitn dot it).

## License

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.