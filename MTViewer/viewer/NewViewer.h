//
//  NewViewer.h
//  viewer
//
//  Created by Paolo Bosetti on 1/14/13.
//  Copyright (c) 2013 UniTN. All rights reserved.
//

#ifndef __viewer__NewViewer__
#define __viewer__NewViewer__

#ifdef __APPLE__
#include <OpenGL/gl.h>     // The GL Header File
#include <GLUT/glut.h>   // The GL Utility Toolkit (Glut) Header
#else
#include <GL/gl.h>     // The GL Header File
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#endif

#include <iostream>
#include <string>
#include "Prism.h"


using namespace std;

enum drag_axis_t {
  drag_x = 0,
  drag_y,
  drag_z,
  drag_none
};

typedef struct {
  char    flag;
  bool    run;
  GLfloat set_coord[3];
  GLfloat act_coord[3];
  GLfloat offset[3];
  GLfloat lambda;
  GLfloat S, F;
  GLfloat tool_length;
  GLfloat tool_radius;
} machine_status_t;

typedef struct {
  char flag;
  bool run;
  GLfloat coord[3];
  GLfloat offset[3];
  GLfloat tool_length;
  GLfloat tool_radius;
} command_t;

typedef struct {
  GLint   mouse_button;
  GLint   bmModifiers = 0;
  GLfloat fxDiff = -135;
  GLfloat fYDiff = 30;
  GLfloat fZDiff = 0;
  GLfloat xLastIncr = 0;
  GLfloat yLastIncr = 0;
  GLfloat fScale = 1.0;
  GLfloat view_angle = 65.0f;
  GLfloat xLast = -1;
  GLfloat yLast = -1;
  GLfloat incr = 10;
  GLfloat x_pan = 0.0;
  GLfloat y_pan = 0.0;
  GLfloat LightMultiplier = 1.0;
  GLfloat LightDirection[3] = { -0.57735f, -0.57735f, -0.57735f };
  GLfloat ClearColor[4] = {0.0f, 0.1f, 0.3f, 0.5f};
  bool    lighting = true;
  bool    show_points = true;
  uint16_t width, height;
  GLfloat axes_length = 50;
  drag_axis_t drag_axis = drag_none;
} gl_status;


void
output(int x, int y, string *s);

void
display();

void
reshape (int width , int height);

void
arrow_keys(int a_keys, int x, int y);

void
motion(int x, int y);

void
mouse(int button, int state, int x, int y);

void
idle();

void
keyboard(unsigned char key, int x, int y);

point_t
offset(command_t * const cmd, MachineTool * const mt);

#endif /* defined(__viewer__NewViewer__) */
