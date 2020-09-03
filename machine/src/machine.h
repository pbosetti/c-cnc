#ifndef MACHINE_H
#define MACHINE_H


#include "pid.h"

//  _   _ _   _ _ _ _
// | | | | |_(_) (_) |_ _  _
// | |_| |  _| | | |  _| || |
//  \___/ \__|_|_|_|\__|\_, |
//                      |__/

uint64_t wait_next(uint64_t interval);
uint64_t now_ns();


//    _        _
//   /_\  __ _(_)___
//  / _ \ \ \ / (_-<
// /_/ \_\/_\_\_/__/
//

struct axis {
  data_t x_0, v_0, x, v;
  data_t thrust, mass, res;
  data_t t;
  struct pid *pid;
};

struct axis *axis_new();
void axis_free(struct axis *a);
void axis_go_to(struct axis *a, data_t x);
void axis_forward(struct axis *a, data_t t);
void axis_set_state(struct axis *a, data_t v);
void axis_set_position(struct axis *a, data_t x);
void axis_do_step(struct axis *a, data_t t);
data_t axis_error(struct axis *a);


//  __  __ _______   ___                      _     _            __
// |  \/  |_   _\ \ / (_)_____ __ _____ _ _  (_)_ _| |_ ___ _ _ / _|__ _ __ ___
// | |\/| | | |  \ V /| / -_) V  V / -_) '_| | | ' \  _/ -_) '_|  _/ _` / _/ -_)
// |_|  |_| |_|   \_/ |_\___|\_/\_/\___|_|   |_|_||_\__\___|_| |_| \__,_\__\___|
//
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct {
  char flag;
  bool run;
  float coord[3];
  float offset[3];
  float tool_length;
  float tool_radius;
} viewer_t;

viewer_t *viewer_new(char const path[]);
void viewer_set_positions(viewer_t *v, data_t x, data_t y, data_t z);

//  __  __         _    _
// |  \/  |__ _ __| |_ (_)_ _  ___
// | |\/| / _` / _| ' \| | ' \/ -_)
// |_|  |_\__,_\__|_||_|_|_||_\___|
//

struct machine_config {
  data_t A, D;
  data_t tq;
  data_t error;
  data_t zero[3];
  data_t offset[3];
};

typedef struct lua_State lua_State;

struct machine {
  struct axis *x, *y, *z;
  struct machine_config *cfg;
  lua_State *lua;
  viewer_t *viewer;
};

// Create a new machine
// config_file is the path of the configuration file
struct machine *machine_new(char *config_file);

// Enable the 3D viewer
// path is the path to the viewer executable
void machine_enable_viewer(struct machine *m, char const path[]);

// Release resources
void machine_free(struct machine *m);

// Reset the machine: set machine position to zero speed
void machine_reset(struct machine *m);

// Set the machine set point for the three axes
void machine_go_to(struct machine *m, data_t x, data_t y, data_t z);

// Set the machine position to x,y,z at zero speed
void machine_set_position(struct machine *m, data_t x, data_t y, data_t z);

void machine_set_position_from_viewer(struct machine *m);

// Forward integrate the dynamics for t seconds ahead
void machine_do_step(struct machine *m, data_t t);

// Return current error (distance between current and target position)
data_t machine_error(struct machine *m);

// Return current axes positions
data_t machine_x(struct machine *m);
data_t machine_y(struct machine *m);
data_t machine_z(struct machine *m);

void machine_wait_for_run(struct machine *m);
void machine_wait_for_stop(struct machine *m);


#endif
