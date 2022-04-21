//   __  __            _     _            
//  |  \/  | __ _  ___| |__ (_)_ __   ___ 
//  | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  | |  | | (_| | (__| | | | | | | |  __/
//  |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|
//
#include "machine.h"

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
    // TODO: Parse the INI file
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

data_t machine_A(const machine_t *m) { assert(m); return m->A; }

data_t machine_tq(const machine_t *m) { assert(m); return m->tq; }

point_t *machine_zero(const machine_t *m) { assert(m); return m->zero; }

point_t *machine_offset(const machine_t *m) { assert(m); return m->offset; }

data_t machine_error(const machine_t *m) { assert(m); return m->error; }