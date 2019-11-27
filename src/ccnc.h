//    ____       ____ _   _  ____ 
//   / ___|     / ___| \ | |/ ___|
//  | |   _____| |   |  \| | |    
//  | |__|_____| |___| |\  | |___ 
//   \____|     \____|_| \_|\____|
//                               
// master header file for C-CNC
// This header has to be included in all other library headers

// double inclusion guard
#ifndef CCNC_H
#define CCNC_H

// System includes
// see man asprintf: to access the asprintf function you have to
// #define _GNU_SOURCE, then #include the stdio.h header:
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h> 


// Types definitions
typedef double data_t;
typedef uint32_t index_t;


// end of double inclusion guard
#endif
