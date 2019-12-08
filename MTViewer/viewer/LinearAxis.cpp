//
//  LinearAxis.cpp
//  viewer
//
//  Created by Paolo Bosetti on 1/7/13.
//  Copyright (c) 2013 UniTN. All rights reserved.
//

#include "LinearAxis.h"
#include <stdio.h>

#pragma mark -
#pragma mark METHODS

LinearAxis::LinearAxis(string s)
{
  name = s;
  length = 500.0;
  width  = 100.0;
  height = 50.0;
  cursor_height = 20.0;
  cursor_length = width;
  position = 0.0;
  limiter_left = 0.0;
  limiter_right = 0.0;
  set_color(1.0, 0.0, 0.0, 0.8);
  start_block.set_color(0.5, 0.5, 0.5, 0.8);
  end_block.set_color(0.5, 0.5, 0.5, 0.8);
  cursor.set_color(0.5, 0.5, 0.5, 0.8);
  in_flange.x = in_flange.y = in_flange.z = 0.0;
  out_flange_position.x = out_flange_position.y = out_flange_position.z = 0.0;
  rotation.angle = 0.0;
  rotation.x = 1.0;
  rotation.y = 0.0;
  rotation.z = 0.0;
  fixed_cursor = false;
  setup();
}

LinearAxis::~LinearAxis()
{

}

void
LinearAxis::setup()
{
  way.set_dimension(length, width, height);
  start_block.set_dimension(cursor_length/2.0, width, height);
  end_block.set_dimension(cursor_length/2.0, width, height);
  cursor.set_dimension(cursor_length, width, cursor_height);

  if (fixed_cursor) {
    way.set_handle(-length/2.0, 0.0, cursor_height + height/2.0);
    start_block.set_handle(cursor_length/4.0, 0.0, cursor_height + height/2.0);
    end_block.set_handle(-cursor_length/4.0-length, 0.0, cursor_height + height/2.0);
    cursor.set_handle(0.0, 0.0, cursor_height/2.0);
  }
  else {
    way.set_handle(-length/2.0, 0.0, -height/2.0);
    start_block.set_handle(cursor_length/4.0, 0.0, -height/2.0);
    end_block.set_handle(-cursor_length/4.0-length, 0.0, -height/2.0);
    cursor.set_handle(0.0, 0.0, -cursor_height/2.0-height);
  }
}

void
LinearAxis::draw()
{
  point_t p;
  p.x = p.y = p.z = 0.0;
  update_position();
  glTranslatef(in_flange.x, in_flange.y, in_flange.z);
  draw_frame();
  glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);
  glPushMatrix();
  if (fixed_cursor) {
    p.x = -position;
    way.set_position(p);
    start_block.set_position(p);
    end_block.set_position(p);
  }
  else {
    p.x = position;
    cursor.set_position(p);
  }
  start_block.draw();
  end_block.draw();
  way.draw();
  cursor.draw();
  glPopMatrix();
  point_t of = out_flange();
  draw_frame(&of);
}

point_t
LinearAxis::out_flange()
{
  point_t p;
  if (fixed_cursor) {
    p.x = length-position + out_flange_position.x;
    p.y = out_flange_position.y;
    p.z = -(height + cursor_height) + out_flange_position.z;
  }
  else {
    p.x = position+out_flange_position.x;
    p.y = out_flange_position.y;
    p.z = height + cursor_height + out_flange_position.z;
  }
  return p;
}

#pragma mark -
#pragma mark GETTERS

GLfloat LinearAxis::get_cursor_height() { return cursor_height; }


#pragma mark -
#pragma mark SETTERS

void LinearAxis::set_color(GLfloat R, GLfloat G, GLfloat B, GLfloat T)
{
  way.set_color(R, G, B, T);
}

void LinearAxis::set_position(GLfloat val)
{
  if (command.setpoint != val) {
    command.start_point = position;
    command.setpoint = val;
    command.set_time = high_resolution_clock::now();
  }
}

void LinearAxis::update_position() {
  // if (position != command.setpoint) {
  //   auto now = high_resolution_clock::now();
  //   auto elapsed = duration_cast<microseconds>(now - command.set_time).count()/1E6f;
  //
  //   // IMPLEMENT DYNAMICS HERE!!!
  //   GLfloat temp = command.start_point + sign(command.setpoint - position) * params.max_speed * elapsed;
  //   if (abs(temp-command.start_point) < abs(command.setpoint-command.start_point)) {
  //     position = temp;
  //   }
  //   else {
  //     position = command.setpoint;
  //   }
  //
  //   if (position <= limiter_left)
  //     position = limiter_left;
  //   else if (position >= length - limiter_right)
  //     position = length - limiter_right;
  // }
  position = command.setpoint;
}

void LinearAxis::set_rotation(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
{
  rotation.angle = a;
  rotation.x = x;
  rotation.y = y;
  rotation.z = z;
}

void LinearAxis::set_in_flange(point_t p)
{
  in_flange = p;
}

void LinearAxis::set_in_flange(GLfloat x, GLfloat y, GLfloat z)
{
  in_flange.x = x;
  in_flange.y = y;
  in_flange.z = z;
}

void LinearAxis::set_out_flange_position(GLfloat x, GLfloat y, GLfloat z)
{
  out_flange_position.x = x;
  out_flange_position.y = y;
  out_flange_position.z = z;
}

void LinearAxis::set_limiters(GLfloat l, GLfloat r)
{
  limiter_left = l;
  limiter_right = r;
}


#pragma mark -
#pragma mark PIVATE METHODS

void LinearAxis::draw_frame()
{
  point_t p;
  p.x = 0.0f; p.y = 0.0f; p.z = 0.0f;
  draw_frame(&p);
}

void LinearAxis::draw_frame(point_t const *p)
{
  glPushMatrix();
  glTranslatef(p->x, p->y, p->z);
  glLineWidth(3.0);
  glColor3f(1, 0, 0);
  glBegin(GL_LINES);
  glVertex3f(-10,0,0);
  glVertex3f(10,0,0);
  glEnd();

  glColor3f(0, 1, 0);
  glBegin(GL_LINES);
  glVertex3f(0,-10,0);
  glVertex3f(0,10,0);
  glEnd();

  glColor3f(0, 0, 1);
  glBegin(GL_LINES);
  glVertex3f(0,0,-10);
  glVertex3f(0,0,10);
  glEnd();

  glPopMatrix();
}
