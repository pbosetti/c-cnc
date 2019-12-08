//
//  MachineTool.cpp
//  viewer
//
//  Created by Paolo Bosetti on 12/17/12.
//  Copyright (c) 2012 UniTN. All rights reserved.
//

#include "MachineTool.h"

MachineTool::MachineTool()
{
  name = "Bosch";
  draw_box  = true;
  draw_axes = true;
  tool_length = 30.0;
  tool_radius = 5.0;
  tool_ballpoint = false;
  range.x = 800;
  range.y = 400;
  range.z = 400;

  x_dim.x = 800;
  x_dim.y = 200;
  x_dim.z = 100;

  y_dim.x = 500;
  y_dim.y = 100;
  y_dim.z = 100;

  z_dim.x = 400;
  z_dim.y = 100;
  z_dim.z = 100;

  x_axis = LinearAxis("X");
  x_axis.length = x_dim.x;
  x_axis.width  = x_dim.y;
  x_axis.height = x_dim.z;
  x_axis.set_out_flange_position(0, -y_dim.y/2.0, 0.0);
  x_axis.setup();

  y_axis = LinearAxis("Y");
  y_axis.length = y_dim.x;
  y_axis.width  = y_dim.y;
  y_axis.height = y_dim.z;
  y_axis.set_color(0.0, 1.0, 0, 0.8);
  y_axis.fixed_cursor = true;
  y_axis.set_rotation(180, 1, -1, 0);
  y_axis.set_out_flange_position(0, y_dim.y/2.0, y_dim.z/2.0);
  y_axis.set_limiters(0, 100);
  y_axis.setup();

  z_axis = LinearAxis("Z");
  z_axis.length = z_dim.x;
  z_axis.width  = z_dim.y;
  z_axis.height = z_dim.z;
  z_axis.set_color(0.0, 0.0, 1.0, 0.8);
  z_axis.set_rotation(120, 1, -1, 1);
  z_axis.fixed_cursor = true;
  z_axis.set_out_flange_position(z_dim.y/2.0, 0.0, z_dim.z/2.0);
  z_axis.setup();
}

void MachineTool::translate_axes()
{
  
}

void MachineTool::plot_point(point_t point, GLfloat size)
{
  glPushMatrix();
  glRotatef(180.0, 0, 1, 0);
  glRotatef(-90.0, 1, 0, 0);
  glColor4f(1,1,1,0);
  glTranslatef(point.x, point.y, point.z-tool_length);
  glutSolidSphere(size, 8, 8);
  glPopMatrix();
}

void
MachineTool::frame_rotation()
{
  glRotatef(180.0, 0, 1, 0);
  glRotatef(-90.0, 1, 0, 0);
}

void MachineTool::display()
{
  glPushMatrix();
  frame_rotation();

  if (draw_box) {
    volume.set_dimension(range.x, range.y, range.z-tool_length);
    volume.wire_only = true;
    volume.set_color(1, 1, 1, 1);
    volume.set_handle(-range.x/2.0, -range.y/2.0, -(range.z-tool_length)/2.0);
    volume.draw();
  }

  // Machine tool
  glPushMatrix();
  glTranslatef(
               y_dim.y/2.0 + z_dim.z/2.0 + z_axis.get_cursor_height(),
               range.y + x_dim.y/2.0 + z_dim.y/2.0,
               range.z - x_dim.z - x_axis.get_cursor_height() - y_axis.get_cursor_height()
  );

  x_axis.draw();

  y_axis.set_in_flange(x_axis.out_flange());
  y_axis.draw();

  z_axis.set_in_flange(y_axis.out_flange());
  z_axis.draw();

  /* Table */

  /* Tool */
  point_t p = z_axis.out_flange();
  GLUquadric *quad;
  quad = gluNewQuadric();
  glColor4f(1,0.5,0,1);
  glTranslatef(p.x, p.y, p.z);
  glRotatef(90, 0, 1, 0);
  glPushMatrix();
  gluCylinder(quad, tool_radius, tool_radius, tool_length - (tool_ballpoint ? tool_radius : 0.0), 20, 1);
  glTranslatef(0.0, 0.0, tool_length - (tool_ballpoint ? tool_radius : 0.0));
  glScalef(1.0, 1.0, (tool_ballpoint ? 1.0 : 0.0));
  gluSphere(quad, tool_radius, 20, 20);
  glPopMatrix();

  glPopMatrix(); // end machine tool

  glPopMatrix();

  glLineWidth(1.0);
  free(quad);
}

point_t MachineTool::get_current_position()
{
  point_t pos;
  pos.x = x_axis.position;
  pos.y = y_axis.position;
  pos.z = z_axis.position;
  return pos;
}

void MachineTool::set_position(point_t pos)
{
  x_axis.set_position(pos.x);
  y_axis.set_position(pos.y);
  z_axis.set_position(pos.z);
}

GLfloat MachineTool::envelope()
{
  GLfloat dims[9] = {
    x_dim.x, x_dim.y, x_dim.z,
    y_dim.x, y_dim.y, y_dim.z,
    z_dim.x, z_dim.y, z_dim.z
  };
  
  GLfloat size = 0.0;

  for (int i = 0; i<9; i++)
    size = dims[i] > size ? dims[i] : size;

  return size;
}


void MachineTool::describe()
{
  cout << "This is Machine Tool mod. " << name << ", envelope: " << envelope() << endl;

  cout << "Position: ";
  cout << "X" << x_axis.position << ", ";
  cout << "Y" << y_axis.position << ", ";
  cout << "Z" << z_axis.position << endl;

  cout << "Draw box: " << (draw_box ? "yes" : "no") << ", ";
  cout << "Draw axes: " << (draw_axes ? "yes" : "no") << endl;
}


