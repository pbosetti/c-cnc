//   __  __            _     _            
//  |  \/  | __ _  ___| |__ (_)_ __   ___ 
//  | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  | |  | | (_| | (__| | | | | | | |  __/
//  |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|
//  Machine class

#ifndef MACHINE_H
#define MACHINE_H

#include "defines.h"
#include "point.h"
#include <mosquitto.h>

//   _____                      
//  |_   _|   _ _ __   ___  ___ 
//    | || | | | '_ \ / _ \/ __|
//    | || |_| | |_) |  __/\__ \
//    |_| \__, | .__/ \___||___/
//        |___/|_|              

// Opaque struct
typedef struct machine machine_t;

//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              

// LIFECYCLE ===================================================================

machine_t *machine_new(const char *ini_path);
void machine_free(machine_t *m);

// MQTT COMMUNICATIONS =========================================================

typedef void (* machine_on_message)(struct mosquitto *mqt, void *ud, const struct mosquitto_message *msg);

int machine_connect(machine_t *m, machine_on_message callback);

int machine_sync(machine_t *m, int rapid);

int machine_listen_start(machine_t *m);

int machine_listen_stop(machine_t *m);

void machine_listen_update(machine_t *m);

void machine_disconnect(machine_t *m);

// ACCESSORS ===================================================================

data_t machine_A(const machine_t *m);

data_t machine_tq(const machine_t *m);

data_t machine_max_error(const machine_t *m);

point_t *machine_zero(const machine_t *m);

point_t *machine_offset(const machine_t *m);

point_t *machine_setpoint(const machine_t *m);

point_t *machine_position(const machine_t *m);

data_t machine_error(const machine_t *m);

data_t machine_rt_pacing(const machine_t *m);




#endif // MACHINE_H