# MTViewer - A simple viewer for a 3 axes machine tool

This folder contains the code for compiling an OpenGL viewer for a three axes 
machine tool. To compile the program, do the dollowing:

```sh
# be sure to be in MTViewer dir:
cd MTViewer
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release
make -Cbuild install
```

Now the executable is in the `bin` directory in the C-CNC project folder.

**For the sake of the course, you don't have to care about the code in 
this folder. You only need to build and install it as above.**