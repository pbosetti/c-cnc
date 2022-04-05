//   __  __  ___ _____ _____
//  |  \/  |/ _ \_   _|_   _|
//  | |\/| | | | || |   | |
//  | |  | | |_| || |   | |
//  |_|  |_|\__\_\|_|   |_|
// Simple MQTT client example
#include "../inic.h"
#include <mosquitto.h>
#include <mqtt_protocol.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <time.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

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
  char user[BUFLEN];
  char password[BUFLEN];
  int run, connecting;
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

//   ____  _                   _   _                     _ _               
//  / ___|(_) __ _ _ __   __ _| | | |__   __ _ _ __   __| | | ___ _ __ ___ 
//  \___ \| |/ _` | '_ \ / _` | | | '_ \ / _` | '_ \ / _` | |/ _ \ '__/ __|
//   ___) | | (_| | | | | (_| | | | | | | (_| | | | | (_| | |  __/ |  \__ \
//  |____/|_|\__, |_| |_|\__,_|_| |_| |_|\__,_|_| |_|\__,_|_|\___|_|  |___/
//           |___/                                                         
enum jump_code {EXIT = 1, RESTART };
sigjmp_buf JumpBuffer;
void sig_handler(int signal) {
  switch (signal)
  {
  case SIGINT:
    siglongjmp(JumpBuffer, EXIT);
    break;
  case SIGINFO:
    siglongjmp(JumpBuffer, RESTART);
  default:
    break;
  }
}

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
  userdata_t ud = {.run = 1, .connecting = 1};
  int rc = 0;
  char *buf = NULL;

  signal(SIGINFO, sig_handler);
  signal(SIGINT, sig_handler);
start:
  rc = 0;
  ud.run = 1;
  ud.connecting = 1;

  buf = malloc(1024 * 1024 * 1024);
  memset(buf, 0, 1024 * 1024 * 1024);

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
  ini_get_char(ini, INI_SECTION, "sub_topic", ud.topic, BUFLEN);
  ini_get_uint64_t(ini, INI_SECTION, "delay", &delay);
  ini_get_char(ini, INI_SECTION, "user", ud.user, BUFLEN);
  ini_get_char(ini, INI_SECTION, "password", ud.password, BUFLEN);
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

  // if username and password are present, then authenticate
  if (strlen(ud.user) * strlen(ud.password) > 0) {
    if ((mosquitto_username_pw_set(mqt, ud.user, ud.password)) !=
        MOSQ_ERR_SUCCESS) {
      perror("Error setting credentials");
    }
  } else { // anonymous connection
    printf("Connecting anonymously\n");
  }

  // connect to broker (localhost)
  if ((mosquitto_connect(mqt, ud.broker_addr, ud.broker_port, 10)) !=
      MOSQ_ERR_SUCCESS) {
    perror("Could not connect to broker");
    exit(3);
  }

  // wait for asynchronous connect to happen
  while (ud.connecting) {
    mosquitto_loop(mqt, 0, 1);
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
    rc = sigsetjmp(JumpBuffer, 1);
    if (rc > 0) {
      ud.run = 0;
    }
  }
  printf("Exiting loop\n");

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
  free(buf);
  if (rc == RESTART) {
    goto start;
  }
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
  printf("<- message: %s @ %s: %f\n",  msg->topic, (char *)msg->payload, t);
  if (strcmp((char *)msg->payload, "stop") == 0) {
    ud->run = 0;
  }
  mosquitto_message_copy(ud->msg, msg);
}