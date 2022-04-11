#include "machine.h"
#include "inic.h"



typedef struct machine {
  data_t A;
  data_t tq;
  data_t error;
  point_t *zero;
  point_t *offset;
} machine_t;

machine_t *machine_new(char *ini_path) {
  machine_t *m = (machine_t *)calloc(sizeof(machine_t), 1);
  if (ini_path) {
    void *ini = ini_init(ini_path);
    data_t x, y, z;
    int rc = 0;
    if (!ini) {
      fprintf(stderr, "Cannot open INI file, exiting.\n");
      return NULL;
    }
    rc += ini_get_double(ini, "C-CNC", "A", &m->A);
    rc += ini_get_double(ini, "C-CNC", "error", &m->error);
    rc += ini_get_double(ini, "C-CNC", "tq", &m->tq);
    rc += ini_get_double(ini, "C-CNC", "origin_x", &x);
    rc += ini_get_double(ini, "C-CNC", "origin_y", &y);
    rc += ini_get_double(ini, "C-CNC", "origin_z", &z);
    m->zero = point_new();
    point_set_xyz(m->zero, x, y, z);
    rc += ini_get_double(ini, "C-CNC", "offset_x", &x);
    rc += ini_get_double(ini, "C-CNC", "offset_y", &y);
    rc += ini_get_double(ini, "C-CNC", "offset_z", &z);
    m->offset = point_new();
    point_set_xyz(m->offset, x, y, z);
    if (rc) {
      fprintf(stderr, "Missing %d config parameters!\n", rc);
      return NULL;
    }
  } else {
    m->A = 125;
    m->error = 0.005;
    m->tq = 0.005;
    m->zero = point_new();
    point_set_xyz(m->zero, 0, 0, 0);
    m->offset = point_new();
    point_set_xyz(m->offset, 0, 0, 0);
  }
  return m;
}

void machine_free(machine_t *m) {
  point_free(m->zero);
  point_free(m->offset);
  free(m);
  m = NULL;
}

data_t machine_A(machine_t *m) { return m->A; }

data_t machine_error(machine_t *m) { return m->error; }

data_t machine_tq(machine_t *m) { return m->tq; }

point_t *machine_zero(machine_t *m) { return m->zero; }

point_t *machine_offset(machine_t *m) { return m->offset; }
