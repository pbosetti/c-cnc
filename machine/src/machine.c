#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <ccnc.h>
#include "machine.h"

struct axis *axis_new() {
  struct axis *a = (struct axis *)malloc(sizeof(struct axis));
  assert(a != NULL);
  struct pid *pid = (struct pid *)malloc(sizeof(struct pid));
  assert(pid != NULL);
  a->pid = pid;
  axis_set_state(a, 0, 0);
  return a;
}

void axis_free(struct axis *a) {
  pid_free(a->pid);
  free(a);
}

void axis_go_to(struct axis *a, data_t x) { a->pid->set_point = x; }

void axis_forward(struct axis *a, data_t dt) {
  a->x = (((-1 + exp(-a->res * dt / a->mass)) * a->mass *
               (a->thrust - a->res * a->v / 1000) +
           a->res * (dt * a->thrust + a->res * a->x / 1000)) /
          pow(a->res, 2.0)) *
         1000;
  a->v = ((a->thrust +
           (-a->thrust + a->res * a->v / 1000) / exp((a->res * dt) / a->mass)) /
          a->res) *
         1000;
}

void axis_set_state(struct axis *a, data_t x, data_t v) {
  a->x = x;
  a->v = v;
}

void axis_do_step(struct axis *a, data_t dt) {
  a->pid->actual = a->x;
  a->thrust = pid_output(a->pid, dt);
  axis_forward(a, dt);
}

data_t axis_error(struct axis *a) { return pid_error(a->pid); }

#if LUA_VERSION_NUM > 501
#define lua_open() luaL_newstate()
#endif

struct machine *machine_new(char *config_file) {
  struct machine *m = malloc(sizeof(struct machine));
  assert(m != NULL);
  m->cfg = malloc(sizeof(struct machine_config));
  assert(m->cfg != NULL);
  m->x = axis_new();
  m->y = axis_new();
  m->z = axis_new();
  // Lua config file
  m->lua = lua_open();
  luaL_openlibs(m->lua);
  if (luaL_dofile(m->lua, config_file) == 1) {
    fprintf(stderr, "Lua error running %s:\n%s\n", config_file,
            lua_tostring(m->lua, -1));
    return NULL;
  }
  lua_getglobal(m->lua, "machine");
  lua_getfield(m->lua, -1, "cfg");
  lua_getfield(m->lua, -1, "A");
  m->cfg->A = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "D");
  m->cfg->D = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "tq");
  m->cfg->tq = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "error");
  m->cfg->error = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "zero");
  lua_rawgeti(m->lua, -1, 1);
  m->cfg->zero[0] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_rawgeti(m->lua, -1, 2);
  m->cfg->zero[1] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_rawgeti(m->lua, -1, 3);
  m->cfg->zero[2] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 3);

  lua_getfield(m->lua, -1, "x");
  lua_getfield(m->lua, -1, "mass");
  m->x->mass = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "res");
  m->x->res = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "pid");
  lua_getfield(m->lua, -1, "kp");
  m->x->pid->kp = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "ki");
  m->x->pid->ki = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "kd");
  m->x->pid->kd = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "saturation");
  lua_rawgeti(m->lua, -1, 1);
  m->x->pid->saturation[0] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_rawgeti(m->lua, -1, 2);
  m->x->pid->saturation[1] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 4);

  lua_getfield(m->lua, -1, "y");
  lua_getfield(m->lua, -1, "mass");
  m->y->mass = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "res");
  m->y->res = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "pid");
  lua_getfield(m->lua, -1, "kp");
  m->y->pid->kp = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "ki");
  m->y->pid->ki = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "kd");
  m->y->pid->kd = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "saturation");
  lua_rawgeti(m->lua, -1, 1);
  m->y->pid->saturation[0] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_rawgeti(m->lua, -1, 2);
  m->y->pid->saturation[1] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 4);

  lua_getfield(m->lua, -1, "z");
  lua_getfield(m->lua, -1, "mass");
  m->z->mass = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "res");
  m->z->res = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "pid");
  lua_getfield(m->lua, -1, "kp");
  m->z->pid->kp = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "ki");
  m->z->pid->ki = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "kd");
  m->z->pid->kd = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_getfield(m->lua, -1, "saturation");
  lua_rawgeti(m->lua, -1, 1);
  m->z->pid->saturation[0] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 1);
  lua_rawgeti(m->lua, -1, 2);
  m->z->pid->saturation[1] = luaL_checknumber(m->lua, -1);
  lua_pop(m->lua, 4);

  m->viewer = NULL;

  return m;
}

void machine_enable_viewer(struct machine *m, char const path[]) {
  assert(m != NULL);
  m->viewer = viewer_new(path);
  viewer_set_positions(m->viewer, m->x->x, m->y->x, m->z->x);
}

void machine_free(struct machine *m) {
  assert(m != NULL);
  axis_free(m->x);
  axis_free(m->y);
  axis_free(m->z);
  free(m->cfg);
  lua_close(m->lua);
  free(m);
}

void machine_reset(struct machine *m) {
  assert(m != NULL);
  axis_set_state(m->x, 0.0, 0.0);
  axis_set_state(m->y, 0.0, 0.0);
  axis_set_state(m->z, 0.0, 0.0);
}

void machine_set_position(struct machine *m, data_t x, data_t y, data_t z) {
  assert(m != NULL);
  axis_set_state(m->x, x, 0.0);
  axis_set_state(m->y, y, 0.0);
  axis_set_state(m->z, z, 0.0);
  viewer_set_positions(m->viewer, m->x->x, m->y->x, m->z->x);
}

void machine_set_position_from_viewer(struct machine *m) {
  machine_set_position(m, m->viewer->coord[0], m->viewer->coord[1], m->viewer->coord[2]);
}

void machine_go_to(struct machine *m, data_t x, data_t y, data_t z) {
  assert(m != NULL);
  axis_go_to(m->x, x);
  axis_go_to(m->y, y);
  axis_go_to(m->z, z);
}

void machine_do_step(struct machine *m, data_t t) {
  assert(m != NULL);
  axis_do_step(m->x, t);
  axis_do_step(m->y, t);
  axis_do_step(m->z, t);
  viewer_set_positions(m->viewer, m->x->x, m->y->x, m->z->x);
}

data_t machine_error(struct machine *m) {
  data_t ex = axis_error(m->x);
  assert(m != NULL);
  data_t ey = axis_error(m->y);
  data_t ez = axis_error(m->z);
  return sqrt(pow(ex, 2.0) + pow(ey, 2.0) + pow(ez, 2.0));
}

void machine_wait_for_run(struct machine *m) {
  assert(m != NULL);
  if (m->viewer) {
    while (m->viewer->run == 0) { usleep(1E5); }
  }
}

void machine_wait_for_stop(struct machine *m) {
  assert(m != NULL);
  if (m->viewer) {
    while (m->viewer->run != 0) { usleep(1E5); }
  }
}

data_t machine_x(struct machine *m) {
  return m->x->x;
}

data_t machine_y(struct machine *m) {
  return m->y->x;
}

data_t machine_z(struct machine *m) {
  return m->z->x;
}


//  __  __ _______   ___                      _     _            __
// |  \/  |_   _\ \ / (_)_____ __ _____ _ _  (_)_ _| |_ ___ _ _ / _|__ _ __ ___
// | |\/| | | |  \ V /| / -_) V  V / -_) '_| | | ' \  _/ -_) '_|  _/ _` / _/ -_)
// |_|  |_| |_|   \_/ |_\___|\_/\_/\___|_|   |_|_||_\__\___|_| |_| \__,_\__\___|
//

viewer_t *viewer_new(char const path[]) {
  viewer_t *cmd = NULL;
  int shmid = 0;
  key_t key;
  key = ftok(path, 1);
  // fork and exec the executable at path
  if (fork() == 0) {
    execl(path, path, (char *)NULL);
    perror("execl");
    return NULL;
  }
  // connect to the shared memory at key
  while ((shmid = shmget(key, sizeof(viewer_t), 0666)) == -1) {
    usleep(1E4);
    fprintf(stderr, ".");
  }
  // map the shared memory to the cmd struct
  if ((cmd = (viewer_t *)shmat(shmid, 0, SHM_RND)) == NULL) {
    perror("shmat");
    return NULL;
  }
  return cmd;
}

void viewer_set_positions(viewer_t *v, data_t x, data_t y, data_t z) {
  if (v != NULL) {
    v->flag = '*';
    v->coord[0] = (float)x;
    v->coord[1] = (float)y;
    v->coord[2] = (float)z;
  }
}
