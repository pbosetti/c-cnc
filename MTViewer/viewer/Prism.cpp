//
//  Prism.cpp
//  viewer
//
//  Created by Paolo Bosetti on 12/28/12.
//  Copyright (c) 2012 UniTN. All rights reserved.
//

#include "Prism.h"

Prism::Prism(uint16_t par_id, string par_name)
{
  id = par_id;
  name = par_name;
  draw_handles = false;
  wire_only = false;
  center.x = center.y = center.z = 0.0;
  position.x = position.y = position.z = 0.0;
  dimension.x = dimension.y = dimension.z = 10.0;
  color.R = color.G = color.B = 0.5;
  color.T = 0.5;
  handles.push_back(center);
  handle_id = 0;
}

Prism::~Prism()
{

}

void Prism::set_center(point_t c)
{
  center = c;
}

void
Prism::set_center(GLfloat x, GLfloat y, GLfloat z)
{
  center.x = x;
  center.y = y;
  center.z = z;
}

point_t Prism::get_center()
{
  return center;
}

void Prism::set_dimension(point_t d)
{
  dimension = d;
}

void Prism::set_dimension(GLfloat x, GLfloat y, GLfloat z)
{
  dimension.x = x;
  dimension.y = y;
  dimension.z = z;
}

point_t Prism::get_dimension()
{
  return dimension;
}

void Prism::set_position(point_t p)
{
  position = p;
}

void Prism::set_position(GLfloat x, GLfloat y, GLfloat z)
{
  position.x = x;
  position.y = y;
  position.z = z;
}


void Prism::add_handle(point_t h)
{
  handles.push_back(h);
}

void Prism::add_handle(GLfloat x, GLfloat y, GLfloat z)
{
  point_t p;
  p.x = x;
  p.y = y;
  p.z = z;
  add_handle(p);
}

void Prism::set_handle(GLfloat x, GLfloat y, GLfloat z)
{
  handles[0].x = x;
  handles[0].y = y;
  handles[0].z = z;
  select_handle(0);
}

vector<point_t> Prism::get_handles()
{
  return handles;
}

point_t Prism::get_handle(uint16_t i)
{
  return handles[i];
}

void Prism::select_handle(uint16_t h)
{
  if (h < 0)
    handle_id = handles.size()-1;
  else
    handle_id = h;
}

void Prism::set_color(GLfloat R, GLfloat G, GLfloat B, GLfloat T)
{
  color.R = R;
  color.G = G;
  color.B = B;
  color.T = T;
}


void Prism::describe()
{
  cout << "Prism #" << id << ": " << name << endl;
  cout << "at " << center.x << "," << center.y << "," << center.z << endl;
  cout << "color " << color.R << "," << color.G << "," << color.B << "," << color.T << endl;
  for(int i=0; i < handles.size(); i++)
    cout << "handle " << i << ": " <<
    handles[i].x << "," << handles[i].y << "," << handles[i].z << endl;
}

void Prism::draw()
{
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);
  glTranslatef(-handles[handle_id].x, -handles[handle_id].y, -handles[handle_id].z);
  glScalef(dimension.x, dimension.y, dimension.z);
  glColor4f(color.R, color.G, color.B, color.T);
  if (! wire_only)
    glutSolidCube(1.0);
  glLineWidth(2.0);
  glColor4f(color.R, color.G, color.B, 1.0);
  glutWireCube(1.0);
  glPopMatrix();
}