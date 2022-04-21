//   ____       _       _   
//  |  _ \ ___ (_)_ __ | |_ 
//  | |_) / _ \| | '_ \| __|
//  |  __/ (_) | | | | | |_ 
//  |_|   \___/|_|_| |_|\__|
//  Point class

#ifndef POINT_H
#define POINT_H

#include "defines.h"

//   _____                      
//  |_   _|   _ _ __   ___  ___ 
//    | || | | | '_ \ / _ \/ __|
//    | || |_| | |_) |  __/\__ \
//    |_| \__, | .__/ \___||___/
//        |___/|_|              

// Object class defined as an OPAQUE STRUCT
typedef struct point point_t;


//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
// LIFECYCLE ===================================================================

// Create a point
point_t *point_new();

// Free the memory
void point_free(point_t *p);

// Inspection
// WARNING: desc is internally allocated, remember to free() it 
// when done!!!
void point_inspect(const point_t *p, char **desc);

// ACCESSORS ===================================================================

// Set coordinates
void point_set_x(point_t *p, data_t val);
void point_set_y(point_t *p, data_t val);
void point_set_z(point_t *p, data_t val);
void point_set_xyz(point_t *p, data_t x, data_t y, data_t z);

// GETTERS
data_t point_x(const point_t *p);
data_t point_y(const point_t *p);
data_t point_z(const point_t *p);

// COMPUTATION =================================================================

// Distance between two points
data_t point_dist(const point_t *from, const point_t *to);

// Projections
void point_delta(const point_t *from, const point_t *to, point_t *delta);

// "Modal behavior": a point may have undefined coordinates and if so it
// must be able ti inherit undefined coordinates from the previous point
void point_modal(const point_t *from, point_t *to);


#endif // POINT_H