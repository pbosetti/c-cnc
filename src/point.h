//    ____       ____ _   _  ____ 
//   / ___|     / ___| \ | |/ ___|
//  | |   _____| |   |  \| | |    
//  | |__|_____| |___| |\  | |___ 
//   \____|     \____|_| \_|\____|
// Point object in 3D coordinates
#ifndef POINT_H
#define POINT_H

// include master header
#include "ccnc.h"

// DATA STRUCTURES
// ===============

// The point object: three coordinates and a bitmask:
// the bitmask s holds three bits indicating which 
// field has been set or not
typedef struct {
  data_t x, y, z; // coordinates
  uint8_t s;      // set/unset bitmask
} point_t;


// FUNCTIONS
// =========

// create a new point
point_t point_new();

// set point coordinates and update bitmask
void point_x(point_t *p, data_t v);
void point_y(point_t *p, data_t v);
void point_z(point_t *p, data_t v);
void point_xyz(point_t *p, data_t x, data_t y, data_t z);

// calculate distance between p1 and p2 (always positive)
data_t point_dist(point_t *p1, point_t *p2);

// calculates the three projections of a segment
void point_delta(point_t *p1, point_t *p2, point_t *delta);

// modal behavior: copy missing values in p2 from p1
void point_modal(point_t *p1, point_t *p2);

// provide a description of the point
// description as [1.000 - 2.345]
// WARNING: *desc is allocated!
// example usage:
// char *desc;
// point_inspect(pt, &desc);
// ...
// free(desc);
void point_inspect(point_t *p, char **desc);

#endif

