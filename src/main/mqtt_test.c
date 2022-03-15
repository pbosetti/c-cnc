//   __  __  ___ _____ _____
//  |  \/  |/ _ \_   _|_   _|
//  | |\/| | | | || |   | |
//  | |  | | |_| || |   | |
//  |_|  |_|\__\_\|_|   |_|
// Simple MQTT client example
#include "../inic.h"
#include <mosquitto.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <time.h>
#include <unistd.h>

//   ____            _                 _   _                 
//  |  _ \  ___  ___| | __ _ _ __ __ _| |_(_) ___  _ __  ___ 
//  | | | |/ _ \/ __| |/ _` | '__/ _` | __| |/ _ \| '_ \/ __|
//  | |_| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | \__ \
//  |____/ \___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|___/
// 
// Preprocessor macros and constants
#define BUFLEN 1024
#define INI_SECTION "MQTT"
#define INI_FILE "settings.ini"

// Custom types
typedef struct {
  char broker_addr[BUFLEN];
  int broker_port;
  char topic[BUFLEN];
  int run;
  struct mosquitto_message *msg;
} userdata_t;

// Functions
void on_connect(struct mosquitto *mqt, void *obj, int rc);

void on_disconnect(struct mosquitto *mqt, void *obj, int rc);

void on_subscribe(struct mosquitto *mqt, void *obj, int mid, int qos_count,
                  const int *grated_qos);

void on_unsubscribe(struct mosquitto *mqt, void *obj, int mid);

void on_message(struct mosquitto *mqt, void *obj,
                const struct mosquitto_message *msg);

//                   _
//   _ __ ___   __ _(_)_ __
//  | '_ ` _ \ / _` | | '_ \
//  | | | | | | (_| | | | | |
//  |_| |_| |_|\__,_|_|_| |_|
//
int main(int argc, char const *argv[]) {
  struct mosquitto *mqt;
  void *ini = NULL;
  uint64_t delay = 0;
  userdata_t ud = {.run = 1};

  if (argc == 2)
    ini = ini_init(argv[1]);
  else
    ini = ini_init(INI_FILE);

  // if the INI file cannot be read, exit immediately.
  if (!ini) {
    fprintf(stderr, "Cannot open INI file, exiting.\n");
    // exit code: not 0 => error
    return 1;
  }

  ini_get_char(ini, INI_SECTION, "broker_addr", ud.broker_addr, BUFLEN);
  ini_get_int(ini, INI_SECTION, "broker_port", &ud.broker_port);
  ini_get_char(ini, INI_SECTION, "topic", ud.topic, BUFLEN);
  ini_get_uint64_t(ini, INI_SECTION, "delay", &delay);
  printf("Looping with delay %.4f s\n", (float)delay / 1E6);

  // Initialize library
  if (mosquitto_lib_init() != MOSQ_ERR_SUCCESS) {
    perror("Could not initialize library");
    exit(1);
  }

  // check for library version
  {
    int major, minor, rev;
    mosquitto_lib_version(&major, &minor, &rev);
    printf("Version of libmosquitto: %d.%d.%d\n", major, minor, rev);
  }

  // initialize object
  if (!(mqt = mosquitto_new(NULL, 1, &ud))) {
    perror("Could not initialize MQTT");
    exit(2);
  }

  // set callbacks
  mosquitto_connect_callback_set(mqt, on_connect);
  mosquitto_disconnect_callback_set(mqt, on_disconnect);
  mosquitto_subscribe_callback_set(mqt, on_subscribe);
  mosquitto_unsubscribe_callback_set(mqt, on_unsubscribe);
  mosquitto_message_callback_set(mqt, on_message);

  // connect to broker (localhost)
  if ((mosquitto_connect(mqt, ud.broker_addr, ud.broker_port, 10)) !=
      MOSQ_ERR_SUCCESS) {
    perror("Could not connect to broker");
    exit(3);
  }

  // main loop
  while (ud.run) {
    if (mosquitto_loop(mqt, 0, 1) != MOSQ_ERR_SUCCESS) {
      perror("mosquitto loop error");
      break;
    }
    if (delay > 0) {
      usleep(delay);
    }
  }

  // unsubscribe
  mosquitto_unsubscribe(mqt, NULL, ud.topic);

  // wait a little for the unsubscribe message to be delivered
  usleep(1E5);
  // sync socket operations one last time
  mosquitto_loop(mqt, 0, 1);

  // cleanup
  mosquitto_disconnect(mqt);
  mosquitto_destroy(mqt);
  mosquitto_lib_cleanup();
  return 0;
}

//   ____        __ _       _ _   _                 
//  |  _ \  ___ / _(_)_ __ (_) |_(_) ___  _ __  ___ 
//  | | | |/ _ \ |_| | '_ \| | __| |/ _ \| '_ \/ __|
//  | |_| |  __/  _| | | | | | |_| | (_) | | | \__ \
//  |____/ \___|_| |_|_| |_|_|\__|_|\___/|_| |_|___/
// 
void on_connect(struct mosquitto *mqt, void *obj, int rc) {
  userdata_t *ud = obj;
  printf("-> Connected to %s:%d\n", ud->broker_addr, ud->broker_port);
  // subscribe
  if ((mosquitto_subscribe(mqt, NULL, ud->topic, 1)) != MOSQ_ERR_SUCCESS) {
    perror("Could not suscribe");
    exit(4);
  }
}

void on_disconnect(struct mosquitto *mqt, void *obj, int rc) {
  printf("<- Disconnected\n");
}

void on_subscribe(struct mosquitto *mqt, void *obj, int mid, int qos_count,
                  const int *grated_qos) {
  userdata_t *ud = obj;
  printf("-> Subscribed to topic %s\n", ud->topic);
}

void on_unsubscribe(struct mosquitto *mqt, void *obj, int mid) {
  printf("<- Unsubscribed\n");
}

void on_message(struct mosquitto *mqt, void *obj,
                const struct mosquitto_message *msg) {
  userdata_t *ud = obj;
  struct timespec ts;
  double t = 0;
  static double t0 = 0;
  clock_gettime(CLOCK_REALTIME, &ts);
  if (strncmp((char *)msg->payload, "msg#000", msg->payloadlen) == 0) {
    t0 = ts.tv_sec + ts.tv_nsec / 1.0E9;
    t = 0.0;
  } else {
    t = ts.tv_sec + ts.tv_nsec / 1.0E9 - t0;
  }
  printf("<- message: %s @ %f\n", (char *)msg->payload, t);
  printf("   topic: %s\n", msg->topic);
  if (strcmp((char *)msg->payload, "stop") == 0) {
    ud->run = 0;
  }
  mosquitto_message_copy(ud->msg, msg);
}