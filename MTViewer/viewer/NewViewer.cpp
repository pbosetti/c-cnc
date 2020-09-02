//
//  NewViewer.cpp
//  viewer
//
//  Created by Paolo Bosetti on 1/14/13.
//  Copyright (c) 2013 UniTN. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "MachineTool.h"
#include "AntTweakBar.h"
#include "NewViewer.h"

static enum {
  HUD_hidden = 0,
  HUD_top,
  HUD_bottom
} g_head_up_display;

static command_t     * g_status  = new command_t;
static command_t     * g_command = new command_t;
static point_t         g_coord;
static gl_status       g_viewer;
static TwBar         * g_bar; // Pointer to the tweak bar
static GLfloat         g_points_size = 2.0;
static vector<point_t> g_points;
static MachineTool   * g_mt = new MachineTool();

void
output(int x, int y, string *s)
{
  int len, i;
  len = (int) s->length();
  glRasterPos3i(x, y, 0);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, s->at(i));
  }
}

point_t
offset(command_t * const cmd, MachineTool * const mt)
{
  point_t offset;
  offset.x = cmd->coord[0] + cmd->offset[0];
  offset.y = cmd->coord[1] + cmd->offset[1];
  offset.z = cmd->coord[2] + cmd->offset[2] + mt->tool_length;
  return offset;
}


void
display()   // Create The Display Function
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
  glClearColor(g_viewer.ClearColor[0], g_viewer.ClearColor[1], g_viewer.ClearColor[2], g_viewer.ClearColor[3]);
  glClearDepth(100.0);

  if (g_head_up_display == HUD_bottom) TwDraw();

  // Setup 3D scene:
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LINE_SMOOTH);

  float v[4];
  if (g_viewer.lighting) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    v[0] = v[1] = v[2] = g_viewer.LightMultiplier*0.4f; v[3] = 1.0f;
    glLightfv(GL_LIGHT0, GL_AMBIENT, v);
    v[0] = v[1] = v[2] = g_viewer.LightMultiplier*0.8f; v[3] = 1.0f;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
    v[0] = -g_viewer.LightDirection[0]; v[1] = -g_viewer.LightDirection[1]; v[2] = -g_viewer.LightDirection[2]; v[3] = 0.0f;
    glLightfv(GL_LIGHT0, GL_POSITION, v);
  }
  else {
    glDisable(GL_LIGHTING);
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(g_viewer.view_angle,(GLfloat)g_viewer.width/(GLfloat)g_viewer.height,1.0f,20000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, -g_mt->envelope()/4.0, -2.0 * g_mt->envelope()/2.0);
  glTranslatef(g_viewer.x_pan, g_viewer.y_pan, 0.0);

  // trackball rotation and scaling
  glPushMatrix();

  glRotatef(g_viewer.fYDiff, 1,0,0);
  glRotatef(g_viewer.fxDiff, 0,1,0);
  glRotatef(g_viewer.fZDiff, 0,0,1);
  glScalef(g_viewer.fScale, g_viewer.fScale, g_viewer.fScale);

  g_mt->set_position(offset(g_status, g_mt));
  g_mt->display();

  if (g_viewer.show_points) {
    for (int i =0; i < g_points.size(); i++) {
      g_mt->plot_point(g_points[i], g_points_size);
    }
  }

  if (g_mt->draw_axes) {
    g_mt->frame_rotation();
    // Machine frame
    glPushMatrix();
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

    // Local frame
    glTranslatef(g_status->offset[0], g_status->offset[1], g_status->offset[2]);
    glPushMatrix();
    glLineWidth(3.0);
    glColor4f(1, 0, 0, 0.66);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(g_viewer.axes_length,0,0);
    glEnd();
    glPushMatrix();
    glTranslatef(g_viewer.axes_length,0,0);
    glRotatef(90,0,1,0);
    glutSolidCone(g_viewer.axes_length/5.0, g_viewer.axes_length/2.0, 20, 1);
    glPopMatrix();

    glColor4f(0,1,0,0.66);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,g_viewer.axes_length,0);
    glEnd();
    glPushMatrix();
    glTranslatef(0,g_viewer.axes_length,0);
    glRotatef(-90,1,0,0);
    glutSolidCone(g_viewer.axes_length/5.0, g_viewer.axes_length/2.0, 20, 1);
    glPopMatrix();

    glColor4f(0,0,1,0.66);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0,g_viewer.axes_length);
    glEnd();
    glPushMatrix();
    glTranslatef(0,0,g_viewer.axes_length);
    glRotatef(0,0,1,0);
    glutSolidCone(g_viewer.axes_length/5.0, g_viewer.axes_length/2.0, 20, 1);
    glLineWidth(1.0);
    glPopMatrix();
    glPopMatrix();
  }
  glPopMatrix();

  // end of 3D scene


  // Draw 2D scene
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LINE_SMOOTH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, g_viewer.width, 0, g_viewer.height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1, 1, 1);

  char buff[100];
  string s;
  switch (g_viewer.drag_axis) {
    case drag_x:
      s = "Drag X";
      break;
    case drag_y:
      s = "Drag Y";
      break;
    case drag_z:
      s = "Drag Z";
      break;
    default:
      s = "";
      break;
  }
  output(30, g_viewer.height - 28, &s);

  sprintf(buff, "%9s%9s%9s", "Machine", "Local", "Tool");
  s = buff;
  output(30, 48, &s);
  point_t coord = g_mt->get_current_position();
  sprintf(buff, "X%8.3f %8.3f %8s", coord.x, g_status->coord[0], g_mt->tool_ballpoint ? "BALL" : "CYL");
  s = buff;
  output(30, 34, &s);

  sprintf(buff, "Y%8.3f %8.3f L%7.3f", coord.y, g_status->coord[1], g_mt->tool_length);
  s = buff;
  output(30, 22, &s);

  sprintf(buff, "Z%8.3f %8.3f R%7.3f", coord.z, g_status->coord[2], g_mt->tool_radius);
  s = buff;
  output(30, 10, &s);

  // Draw table lines
  glColor3f(0.5, 0.5, 0.5);
  glBegin(GL_LINES);
  glVertex2d(30, 46);
  glVertex2d(30+8*27, 46);
  glEnd();
  glBegin(GL_LINES);
  glVertex2d(32+8*9, 46);
  glVertex2d(32+8*9, 10);
  glEnd();
  glBegin(GL_LINES);
  glVertex2d(32+8*18, 46);
  glVertex2d(32+8*18, 10);
  glEnd();

  glPopMatrix();

  if (g_head_up_display == HUD_top) TwDraw();

  glutSwapBuffers();
}


void
reshape (int width , int height)
{
  if (height==0)
    height=1;

  g_viewer.width = width;
  g_viewer.height = height;

  glViewport(0, 0, g_viewer.width, g_viewer.height);

  // Send the new window size to AntTweakBar
  TwWindowSize(g_viewer.width, g_viewer.height);
}

void
arrow_keys(int a_keys, int x, int y)
{
  if (TwEventSpecialGLUT(a_keys, x, y))
    return;

  switch ( a_keys ) {
    case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
      glutFullScreen ( ); // Go Into Full Screen Mode
      break;
    case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
      glutReshapeWindow ( 500, 500 ); // Go Into A 500 By 500 Window
      break;
    default:
      break;
  }
}

void
motion(int x, int y)
{
  if (TwEventMouseMotionGLUT(x, y)) {
    return;
  }

  if (g_viewer.xLast != -1 || g_viewer.yLast != -1) {
    g_viewer.xLastIncr = x - g_viewer.xLast;
    g_viewer.yLastIncr = y - g_viewer.yLast;
    // ZOOM
    if ((g_viewer.bmModifiers & GLUT_ACTIVE_CTRL) != 0) {
      if (g_viewer.xLast != -1) {
        g_viewer.fScale += (g_viewer.yLastIncr * 0.01);
        if (g_viewer.fScale < 0.1)
          g_viewer.fScale = 0.1;
        if (g_viewer.fScale > 20)
          g_viewer.fScale = 20.0;
      }
    }
    // VIEWING ANGLE
    else if ((g_viewer.bmModifiers & GLUT_ACTIVE_SHIFT) != 0) {
      if (g_viewer.xLast != -1) {
        g_viewer.view_angle += (g_viewer.yLastIncr * 0.1);
        if (g_viewer.view_angle < 6.0)
          g_viewer.view_angle = 6.0;
        if (g_viewer.view_angle > 270.0)
          g_viewer.view_angle = 270.0;
      }
    }
    // PANNING
    else if (g_viewer.mouse_button == GLUT_RIGHT_BUTTON) {
      if (g_viewer.xLast != -1) {
        g_viewer.x_pan += g_viewer.xLastIncr * 2.0;
        g_viewer.y_pan += -g_viewer.yLastIncr * 2.0;
      }
    }
    // DRAG AXES
    else if (g_viewer.drag_axis != drag_none) {
      g_status->coord[g_viewer.drag_axis] += g_viewer.xLastIncr;
    }
    // TRACKBALL
    else {
      if (g_viewer.xLast != -1) {
        g_viewer.fxDiff += g_viewer.xLastIncr;
        g_viewer.fYDiff += g_viewer.yLastIncr;
      }
    }
  }
  g_viewer.xLast = x;
  g_viewer.yLast = y;
  glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
  if (TwEventMouseButtonGLUT(button, state, x, y)) {
    return;
  }

  g_viewer.mouse_button = (GLint)button;
  g_viewer.bmModifiers = glutGetModifiers();

  if (state == GLUT_UP) {
    g_viewer.xLast = -1;
    g_viewer.yLast = -1;
  }
  g_viewer.xLastIncr = 0;
  g_viewer.yLastIncr = 0;

  glutPostRedisplay();
}

void
idle()
{
  if (g_command->flag == '*') {
    g_mt->tool_length = g_command->tool_length;
    g_mt->tool_radius = g_command->tool_radius;
    g_coord.x = g_command->coord[0];
    g_coord.y = g_command->coord[1];
    g_coord.z = g_command->coord[2];
    memcpy(g_status, g_command, sizeof(command_t));
    // g_status  = g_command;
    g_points.push_back(g_coord);
    g_command->flag = '\0';
    TwRefreshBar(g_bar);
  }
  usleep(1000);
  glutPostRedisplay();
}

void setup_tw_machine_vars()
{
  TwRemoveAllVars(g_bar);
  TwAddVarRO(g_bar, "Machine name", TW_TYPE_STDSTRING, &(g_mt->name), NULL);
  TwAddVarRW(g_bar, "Tool length", TW_TYPE_FLOAT, &(g_mt->tool_length), " min=0.0 max=200.0 step=1 keyIncr=l keyDecr=L group='Machine' ");
  TwAddVarRW(g_bar, "Tool radius", TW_TYPE_FLOAT, &(g_mt->tool_radius), " min=0.0 max=200.0 step=1 keyIncr=r keyDecr=R group='Machine' ");
  TwAddVarRW(g_bar, "Ballpoint", TW_TYPE_BOOLCPP, &(g_mt->tool_ballpoint), "group='Machine'");

  TwAddVarRW(g_bar, "X", TW_TYPE_FLOAT, &(g_status->coord[0]), " group='Coordinates'");
  TwAddVarRW(g_bar, "Y", TW_TYPE_FLOAT, &(g_status->coord[1]), " group='Coordinates'");
  TwAddVarRW(g_bar, "Z", TW_TYPE_FLOAT, &(g_status->coord[2]), " group='Coordinates'");
  TwAddVarRW(g_bar, "X0", TW_TYPE_FLOAT, &(g_status->offset[0]), " group='Coordinates'");
  TwAddVarRW(g_bar, "Y0", TW_TYPE_FLOAT, &(g_status->offset[1]), " group='Coordinates'");
  TwAddVarRW(g_bar, "Z0", TW_TYPE_FLOAT, &(g_status->offset[2]), " group='Coordinates'");

  TwAddVarRW(g_bar, "Draw axes", TW_TYPE_BOOLCPP, &(g_mt->draw_axes), " group='Options'");
  TwAddVarRW(g_bar, "Draw box", TW_TYPE_BOOLCPP, &(g_mt->draw_box), " group='Options'");
  TwAddVarRW(g_bar, "Zoom", TW_TYPE_FLOAT, &(g_viewer.fScale), " min=0.1 max=20.0 step=0.01 group='Options'");
  TwAddVarRW(g_bar, "View angle", TW_TYPE_FLOAT, &(g_viewer.view_angle), " min=6.0 max=270 step=0.1 group='Options'");
  TwAddVarRW(g_bar, "Points size", TW_TYPE_FLOAT, &(g_points_size), "min=0 max=10 step=0.1 group='Options'");
  TwAddVarRW(g_bar, "Show points", TW_TYPE_BOOLCPP, &(g_viewer.show_points), "group='Options'");
  TwAddVarRW(g_bar, "Clear color", TW_TYPE_COLOR4F, &(g_viewer.ClearColor), " group='Options'");

  TwAddVarRW(g_bar, "Lighting", TW_TYPE_BOOLCPP, &(g_viewer.lighting), " group='Light'");
  TwAddVarRW(g_bar, "Light direction", TW_TYPE_DIR3F, &(g_viewer.LightDirection), " group='Light'");
  TwAddVarRW(g_bar, "Light multiplier", TW_TYPE_FLOAT, &(g_viewer.LightMultiplier), " min=0 max=2 step=0.1 group='Light'");

}

void
keyboard(unsigned char key, int x, int y)
{
  bool move = false;
  if (TwEventKeyboardGLUT(key, x, y) != 0)
    return;
  else
    switch ( key ) {
      case 'q':
        g_status->run = false;
        exit(0);
        break;
      case 'a':
        g_mt->draw_axes = not g_mt->draw_axes;
        break;
      case 'b':
        g_mt->draw_box = not g_mt->draw_box;
        break;
      case 'x':
        g_status->coord[0] += g_viewer.incr;
        move = true;
        break;
      case 'X':
        g_status->coord[0] -= g_viewer.incr;
        move = true;
        break;
      case 'y':
        g_status->coord[1] += g_viewer.incr;
        move = true;
        break;
      case 'Y':
        g_status->coord[1] -= g_viewer.incr;
        move = true;
        break;
      case 'z':
        g_status->coord[2] += g_viewer.incr;
        move = true;
        break;
      case 'Z':
        g_status->coord[2] -= g_viewer.incr;
        move = true;
        break;
      case 'o': {
        point_t coord = g_mt->get_current_position();
        g_status->offset[0] = coord.x;
        g_status->offset[1] = coord.y;
        g_status->offset[2] = coord.z - g_mt->tool_length;
        g_status->coord[0] = 0.0f;
        g_status->coord[1] = 0.0f;
        g_status->coord[2] = 0.0f;
        break;
      }
      case 'p':
        g_mt->describe();
        break;
      case 'h':
        switch (g_head_up_display) {
          case HUD_bottom:
            g_head_up_display = HUD_hidden;
            break;
          case HUD_hidden:
            g_head_up_display = HUD_top;
            break;
          case HUD_top:
            g_head_up_display = HUD_bottom;
            break;
        }
        break;
      case 'd':
        g_viewer.drag_axis = (drag_axis_t)(drag_axis_t(g_viewer.drag_axis) + 1);
        if (g_viewer.drag_axis > drag_none)
          g_viewer.drag_axis = drag_x;
          move = true;
        break;
      case ' ':
        g_command->run = !g_command->run;
        break;
      case 27:        // When Escape Is Pressed...
        g_status->run = false;
        exit ( 0 );   // Exit The Program
        break;        // Ready For Next Case
      default:        // Now Wrap It Up
        break;
    }
  TwRefreshBar(g_bar);
  if (move) {
    g_points.push_back(g_coord);
  }
  if (move)
    memcpy(g_command, g_status, sizeof(command_t));
  // printf("coords: %f %f %f\n", g_status->coord[0], g_status->coord[1], g_status->coord[2]);
  glutPostRedisplay();
}

int
main(int argc, char** argv)
{
  key_t key;
  int shmid = 0;
  shmid_ds *ds = new shmid_ds;
  key = ftok(argv[0], 1);
  if ((shmid = shmget(key, sizeof(command_t), IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
  }
  g_command = (command_t*)shmat(shmid, 0, SHM_RND);

  g_command->tool_length = 30.0;
  g_command->tool_radius = 5.0;
  g_command->run = false;
  g_command->flag = '\0';
  g_command->coord[0] = 0.0f;
  g_command->coord[1] = 0.0f;
  g_command->coord[2] = 0.0f;
  g_command->offset[0] = 0.0f;
  g_command->offset[1] = 0.0f;
  g_command->offset[2] = 0.0f;
  memcpy(g_status, g_command, sizeof(command_t));
  // g_status = g_command;

  glutInit            ( &argc, argv );
  glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA );
  glutInitWindowSize  ( 500, 500 );
  glutInitWindowPosition(100, 100);
  glutCreateWindow    ( "Machine Tool viewer" );
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.1f, 0.3f, 0.5f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_LINE_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
  glEnable ( GL_COLOR_MATERIAL );
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glutDisplayFunc     ( display );
  glutReshapeFunc     ( reshape );

  TwInit(TW_OPENGL, NULL);

  glutKeyboardFunc    ( keyboard );
  glutSpecialFunc     ( arrow_keys );
  glutMotionFunc      ( motion );
  glutMouseFunc       ( mouse );
  glutIdleFunc        ( idle );
  glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);

  TwGLUTModifiersFunc(glutGetModifiers);

  g_bar = TwNewBar("Controls");
  g_head_up_display = HUD_top;
  TwDefine("Controls iconified=true");
  TwDefine("Controls size='200 400' color='96 216 224' ");

  setup_tw_machine_vars();

  glutMainLoop();
  TwTerminate();
  shmdt(g_command);
  if (shmctl(shmid, IPC_RMID, ds) < 0) {
    perror("shmctl");
    exit(1);
  }
  return 0;
}
