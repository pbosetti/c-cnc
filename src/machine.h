
//   __  __            _     _
//  |  \/  | __ _  ___| |__ (_)_ __   ___
//  | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  | |  | | (_| | (__| | | | | | | |  __/
//  |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|
//
#ifndef MACHINE_H
#define MACHINE_H

#include "defines.h"
#include "point.h"

// opaque struct
typedef struct machine machine_t;

// Lifecycle
machine_t *machine_new(char *ini_path);
void machine_free(machine_t *mc);

// Getters
data_t machine_A(machine_t *);
data_t machine_D(machine_t *);
data_t machine_error(machine_t *);
data_t machine_tq(machine_t *);
point_t *machine_zero(machine_t *);
point_t *machine_offset(machine_t *);

#endif // MACHINE_H