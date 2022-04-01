//   ____  _                       _            _
//  / ___|| |_ _ __ ___  ___ ___  | |_ ___  ___| |_
//  \___ \| __| '__/ _ \/ __/ __| | __/ _ \/ __| __|
//   ___) | |_| | |  __/\__ \__ \ | ||  __/\__ \ |_
//  |____/ \__|_|  \___||___/___/  \__\___||___/\__|
// MQTT Stress test
#include "../defines.h"
#include "../inic.h"
#include <mosquitto.h>
#include <mqtt_protocol.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//   ____            _                 _   _
//  |  _ \  ___  ___| | __ _ _ __ __ _| |_(_) ___  _ __  ___
//  | | | |/ _ \/ __| |/ _` | '__/ _` | __| |/ _ \| '_ \/ __|
//  | |_| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | \__ \
//  |____/ \___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|___/
//
// preprocessor macros and constants
#define BUFLEN 1024
#define INI_SECTION "MQTT"
#define INI_FILE "settings.ini"

// Custom types
typedef struct {
  char broker_addr[BUFLEN];
  int broker_port;
  char topic[BUFLEN];
  char user[BUFLEN];
  char password[BUFLEN];
  int run, connecting;
  struct mosquitto_message *msg;
} userdata_t;

// callbacks
void on_connect(struct mosquitto *mqt, void *obj, int rc);

//                   _
//   _ __ ___   __ _(_)_ __
//  | '_ ` _ \ / _` | | '_ \
//  | | | | | | (_| | | | | |
//  |_| |_| |_|\__,_|_|_| |_|
//
int main(int argc, char const *argv[]) {
  struct mosquitto *mqt;
  void *ini = NULL;
  userdata_t ud = {.run = 1, .connecting = 1};
  char pl[BUFLEN] = "";
  int i = 0;

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
  ini_get_char(ini, INI_SECTION, "pub_topic", ud.topic, BUFLEN);
  ini_get_char(ini, INI_SECTION, "user", ud.user, BUFLEN);
  ini_get_char(ini, INI_SECTION, "password", ud.password, BUFLEN);

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

  // if username and password are present, then authenticate
  if (strlen(ud.user) * strlen(ud.password) > 0) {
    if ((mosquitto_username_pw_set(mqt, ud.user, ud.password)) !=
        MOSQ_ERR_SUCCESS) {
      perror("Error setting credentials");
    }
  } else { // anonymous connection
    printf("Connecting anonymously\n");
  }

  // start mosquitto threaded loop
  mosquitto_loop_start(mqt);

  // connect to broker (localhost)
  if ((mosquitto_connect(mqt, ud.broker_addr, ud.broker_port, 10)) !=
      MOSQ_ERR_SUCCESS) {
    perror("Could not connect to broker");
    exit(3);
  }

  // wait for asynchronous connect to happen
  while (ud.connecting) {}

  // main loop
  while (ud.run && i < 100) {
    sprintf(pl, "msg#%03d", i);
    mosquitto_publish(mqt, NULL, ud.topic, strlen(pl), pl, 0, 0);
    i++;
  }

  // wait for publishing to be done
  while (mosquitto_want_write(mqt)) { 
    // sleep must be larger than the average time needed for a publish op
    usleep(10000);  
  }

  // cleanup
  mosquitto_disconnect(mqt);
  mosquitto_destroy(mqt);
  mosquitto_lib_cleanup();
  return 0;
}

void on_connect(struct mosquitto *mqt, void *obj, int rc) {
  userdata_t *ud = obj;
  if (rc == CONNACK_ACCEPTED) {
    printf("-> Connected to %s:%d\n", ud->broker_addr, ud->broker_port);
    // subscribe
    if ((mosquitto_subscribe(mqt, NULL, ud->topic, 1)) != MOSQ_ERR_SUCCESS) {
      perror("Could not suscribe");
      exit(4);
    }
  } else {
    printf("-X Connection error: %s\n", mosquitto_connack_string(rc));
    ud->run = 0;
  }
  ud->connecting = 0;
}