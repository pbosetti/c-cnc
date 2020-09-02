#ifndef PID_H
#define PID_H
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

/* Types for data and indexes: */
typedef double data_t;
typedef uint32_t index_t;

struct pid {
  data_t kp, ki, kd;
  data_t p, i, d;
  data_t output;
  int saturate;
  data_t saturation[2];
  data_t set_point;
  data_t actual;
  data_t prev_error;
};


struct pid *pid_init(data_t kp, data_t ki, data_t kd);
void pid_free(struct pid *pid);
void pid_setup(struct pid *pid, data_t kp, data_t ki, data_t kd);
void pid_reset(struct pid *pid);
data_t pid_error(struct pid *pid);
data_t pid_output(struct pid *pid, data_t dt);


#endif
