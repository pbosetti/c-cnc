//
//  MachineTool.h
//  viewer
//
//  Created by Paolo Bosetti on 12/17/12.
//  Copyright (c) 2012 UniTN. All rights reserved.
//

#ifndef __viewer__MachineTool__
#define __viewer__MachineTool__

#include "LinearAxis.h"

class MachineTool {
  // point_t coord;
  point_t x_dim;
  point_t y_dim;
  point_t z_dim;
//  GLfloat head_dim[3];
//  GLfloat table_dim[3];

public:
  string name;
  bool draw_box;
  bool draw_axes;
  GLfloat axes_length;
  GLfloat tool_length;
  GLfloat tool_radius;
  bool tool_ballpoint;
  point_t range;
  Prism volume;
  LinearAxis x_axis, y_axis, z_axis;

  MachineTool();

  void display();
  void describe();
  void translate_axes();
  void frame_rotation();
  void plot_point(point_t point, GLfloat size);
  point_t get_current_position();
  void set_position(point_t pos);
  GLfloat envelope();

};

#endif /* defined(__viewer__MachineTool__) */
