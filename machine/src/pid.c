#include "pid.h"

// struct pid {
//   data_t kp, ki, kd;
//   data_t p, i, d;
//   int saturate;
//   data_t saturation[2];
//   data_t set_point;
//   data_t actual;
//   data_t prev_error;
// }

struct pid *pid_init(data_t kp, data_t ki, data_t kd) {
  struct pid *pid = malloc(sizeof(struct pid));
  pid_setup(pid, kp, ki, kd);
  return pid;
}

void pid_free(struct pid *pid) {
  free(pid);
}

void pid_setup(struct pid *pid, data_t kp, data_t ki, data_t kd) {
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid_reset(pid);
}

void pid_reset(struct pid *pid) {
  pid->p = 0.0;
  pid->i = 0.0;
  pid->d = 0.0;
  pid->set_point = 0.0;
  pid->actual = 0.0;
  pid->saturate = 0;
  pid->saturation[0] = 0.0;
  pid->saturation[1] = 0.0;
  pid->prev_error = 0.0;
  pid->output = 0.0;
}


data_t pid_error(struct pid *pid) { return pid->set_point - pid->actual; }

data_t pid_output(struct pid *pid, data_t dt) {
  data_t output, e = pid_error(pid);
  pid->p = pid->kp * e;
  pid->i = pid->i + pid->ki * e * dt;
  pid->d = dt != 0 ? pid->kd * (e + pid->prev_error) / dt : 0.0;
  output = pid->p + pid->i + pid->d;
  pid->saturate = 0;
  if (pid->saturation[0] != pid->saturation[1]) {
    if (output > pid->saturation[1]) {
      pid->saturate = 1;
      pid->i = pid->saturation[1] - pid->p - pid->d;
      if (pid->i < 0)
        pid->i = 0.0;
    } else if (output < pid->saturation[0]) {
      pid->saturate = 1;
      pid->i = pid->saturation[0] - pid->p - pid->d;
      if (pid->i > 0)
        pid->i = 0.0;
    }
    output = pid->p + pid->i + pid->d;
    if (output > pid->saturation[1])
      output = pid->saturation[1];
    else if (output < pid->saturation[0])
      output = pid->saturation[0];
  }
  pid->prev_error = e;
  pid->output = output;
  return output;
}
