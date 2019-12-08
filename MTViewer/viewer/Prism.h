//
//  Prism.h
//  viewer
//
//  Created by Paolo Bosetti on 12/28/12.
//  Copyright (c) 2012 UniTN. All rights reserved.
//

#ifndef __viewer__Prism__
#define __viewer__Prism__

#ifdef __APPLE__
#include <OpenGL/gl.h>     // The GL Header File
#include <GLUT/glut.h>   // The GL Utility Toolkit (Glut) Header
#else
#include <GL/gl.h>     // The GL Header File
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#endif

#include <iostream>
#include <vector>

using namespace std;

typedef struct{
  GLfloat x;
  GLfloat y;
  GLfloat z;
} point_t;

typedef struct {
  GLfloat R, G, B, T;
} color_t;

class Prism {
  uint16_t id;
  string name;
  point_t center;
  point_t position;
  point_t dimension;
  color_t color;
  vector<point_t> handles;
  uint16_t handle_id;
  bool draw_handles;

public:
  bool wire_only;

  Prism(uint16_t par_id=0, string par_name="Default name");
  ~Prism();
  void draw();
  void describe();
  void set_center(point_t c);
  void set_center(GLfloat x, GLfloat y, GLfloat z);
  point_t get_center();
  void set_dimension(point_t d);
  void set_dimension(GLfloat x, GLfloat y, GLfloat z);
  point_t get_dimension();
  void set_position(point_t p);
  void set_position(GLfloat x, GLfloat y, GLfloat z);
  void add_handle(point_t h);
  void add_handle(GLfloat x, GLfloat y, GLfloat z);
  void set_handle(GLfloat x, GLfloat y, GLfloat z);
  vector<point_t> get_handles();
  point_t get_handle(uint16_t i);
  void select_handle(uint16_t h=0);
  void set_color(GLfloat R, GLfloat G, GLfloat B, GLfloat T);
};


#endif /* defined(__viewer__Prism__) */
