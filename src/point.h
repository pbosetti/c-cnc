//   ____       _       _
//  |  _ \ ___ (_)_ __ | |_
//  | |_) / _ \| | '_ \| __|
//  |  __/ (_) | | | | | |_
//  |_|   \___/|_|_| |_|\__|
//  Point class
#ifndef POINT_H
#define POINT_H

#include "defines.h"

// We are using a bitmask for encoding the coordinates that are left
// undefined.
// 0000 0000 => none set (0)
// 0000 0001 => x is set (1)
// 0000 0010 => y is set (2)
// 0000 0100 => z is set (3)
// 0000 0111 => xyz set (7)

typedef struct point point_t;

//   _____                 _   _
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/

// Create a point
point_t *point_new();

void point_free(point_t *p);

// Set coordinates
void point_set_x(point_t *p, data_t val);
void point_set_y(point_t *p, data_t val);
void point_set_z(point_t *p, data_t val);
void point_set_xyz(point_t *p, data_t x, data_t y, data_t z);

data_t point_x(point_t *p);
data_t point_y(point_t *p);
data_t point_z(point_t *p);

// Distance between two points
data_t point_dist(point_t *from, point_t *to);

// projections
void point_delta(point_t *from, point_t *to, point_t *delta);

// Inspection
void point_inspect(point_t *p, char **desc);

// "Modal behavior": a point may have undefined coordinates and if so it
// must be able ti inherit undefined coordinates from the previous point
void point_modal(point_t *from, point_t *to);

#endif // POINT_H