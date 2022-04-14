//   __  __            _     _            
//  |  \/  | __ _  ___| |__ (_)_ __   ___ 
//  | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  | |  | | (_| | (__| | | | | | | |  __/
//  |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|
//  Machine class

#ifndef MACHINE_H
#define MACHINE_H

#include "defines.h"
#include "point.h"

// opaque struct
typedef struct machine machine_t;

// Lifecycle
machine_t *machine_new(const char *ini_path);
void machine_free(machine_t *m);

// Accessors
data_t machine_A(const machine_t *m);

data_t machine_tq(const machine_t *m);

point_t *machine_zero(const machine_t *m);

point_t *machine_offset(const machine_t *m);

data_t machine_error(const machine_t *m);




#endif // MACHINE_H