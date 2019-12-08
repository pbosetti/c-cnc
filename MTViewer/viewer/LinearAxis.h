//
//  LinearAxis.h
//  viewer
//
//  Created by Paolo Bosetti on 1/7/13.
//  Copyright (c) 2013 UniTN. All rights reserved.
//

#ifndef __viewer__LinearAxis__
#define __viewer__LinearAxis__

#include <iostream>
#include <chrono>
#include <cmath>
#include "Prism.h"

#define sign(x) (( x > 0 ) - ( x < 0 ))

using namespace std;
using namespace std::chrono;

typedef struct {
  float mass;
  GLfloat max_speed = 1000.0;
} linear_axis_dynamics_t;

typedef struct {
  GLfloat setpoint    = 0.0;
  GLfloat start_point = 0.0;
  high_resolution_clock::time_point set_time = high_resolution_clock::now();
} linear_axis_command_t;

typedef struct {
  GLfloat angle, x, y, z;
} rot_t;

class LinearAxis {
  Prism   way, start_block, end_block, cursor;
  GLfloat limiter_left, limiter_right;
  point_t in_flange;
  point_t out_flange_position;
  rot_t   rotation;
  linear_axis_command_t command;

public:
  string name;
  GLfloat position;
  GLfloat length, width, height;
  GLfloat cursor_length, cursor_height;
  bool   fixed_cursor;
  linear_axis_dynamics_t params;

  LinearAxis(string s="");
  ~LinearAxis();
  void setup();
  void draw();

  point_t out_flange();

  GLfloat get_cursor_height();

  void set_color(GLfloat R, GLfloat G, GLfloat B, GLfloat T);
  void set_position(GLfloat val);
  void update_position();
  void set_rotation(GLfloat a, GLfloat x, GLfloat y, GLfloat z);
  void set_in_flange(point_t p);
  void set_in_flange(GLfloat x, GLfloat y, GLfloat z);
  void set_out_flange_position(GLfloat x, GLfloat y, GLfloat z);
  void set_limiters(GLfloat l, GLfloat r);

private:
  void draw_frame();
  void draw_frame(point_t const *p);
};


#endif /* defined(__viewer__LinearAxis__) */
