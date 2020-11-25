// this is header for the point object
#ifndef POINT_H
#define POINT_H

// master include
#include "ccnc.h"

// A 3-D point object: it has three coordinates and a bitmask field
// for tracking which coordinate is set and which is unset
// according to the following bitmask:
// 0000 0000 => none set (0)
// 0000 0001 => X set (1)
// 0000 0010 => Y set (2)
// 0000 0100 => Z set (4)
// 0000 0111 => ALL set (7)
typedef struct {
  data_t x, y, z;
  uint8_t s;
} point_t;

//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
//  create a point
point_t point_new();

// setting coordinates:
void point_x(point_t *p, data_t val);
void point_y(point_t *p, data_t val);
void point_z(point_t *p, data_t val);
void point_xyz(point_t *p, data_t x, data_t y, data_t z);

// distance between two points
data_t point_dist(point_t *p1, point_t *p2);

// projections
void point_delta(point_t *p1, point_t *p2, point_t *delta);

// modal behaviour:
// copies values from p1 that are missing in p2
void point_modal(point_t *p1, point_t *p2);

// get the desription of the point as a string
void point_inspect(point_t *p, char **desc);

#endif