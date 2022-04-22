//   __  __            _     _            
//  |  \/  | __ _  ___| |__ (_)_ __   ___ 
//  | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  | |  | | (_| | (__| | | | | | | |  __/
//  |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|
//
#include "machine.h"
#include "inic.h"

//   ____            _                 _   _                 
//  |  _ \  ___  ___| | __ _ _ __ __ _| |_(_) ___  _ __  ___ 
//  | | | |/ _ \/ __| |/ _` | '__/ _` | __| |/ _ \| '_ \/ __|
//  | |_| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | \__ \
//  |____/ \___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|___/
                                                          

typedef struct machine {
  data_t A, tq, error;
  point_t *zero, *offset;
} machine_t;


//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
// LIFECYCLE ===================================================================

// Create a new instance reading data from an INI file
// If the INI file is not given (NULL), provide sensible default values
machine_t *machine_new(const char *ini_path) {
  machine_t *m = (machine_t *)calloc(1, sizeof(machine_t));
  if (!m) {
    perror("Error creating machine object");
    exit(EXIT_FAILURE);
  }
  if (ini_path) { // load values from INI file
    void *ini = ini_init(ini_path);
    data_t x, y, z;
    int rc = 0;
    if (!ini) {
      fprintf(stderr, "Could not open the ini file %s\n", ini_path);
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
    ini_free(ini);
    if (rc > 0) {
      fprintf(stderr, "Missing/wrong %d config parameters\n", rc);
      return NULL;
    }
  }
  else { // provide default values
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
  assert(m);
  point_free(m->zero);
  point_free(m->offset);
  free(m);
  m = NULL;
}


// ACCESSORS ===================================================================

#define machine_getter(typ, par) \
typ machine_##par(const machine_t *m) { assert(m); return m->par; }

machine_getter(data_t, A);
machine_getter(data_t, tq);
machine_getter(data_t, error);
machine_getter(point_t *, zero);
machine_getter(point_t *, offset);

