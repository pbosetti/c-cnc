# C programming exercises

## Notes and caveats

* Remember to save the file before compiling (CRTL-S)

## To create a static function

First, compile all the C source files containing functions that you want to include in your library:

gcc -c f1.c f2.c

This creates f1.o and f2.o (object files)

Second, archive the object files into a single library file:

ar rcs mylib.a f1.o f2.o

Third, write a header file that contains the declarations of all the functions in the library (only the signature of each function), and #include that header in your main file.

Lastly, compile the main file by passing to the compiler also the library name:

gcc ex1.c mylib.a -o ex1