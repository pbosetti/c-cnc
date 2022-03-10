#include <mosquitto.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>
#include <string.h>

//       _       __ _       _ _   _                 
//    __| | ___ / _(_)_ __ (_) |_(_) ___  _ __  ___ 
//   / _` |/ _ \ |_| | '_ \| | __| |/ _ \| '_ \/ __|
//  | (_| |  __/  _| | | | | | |_| | (_) | | | \__ \
//   \__,_|\___|_| |_|_| |_|_|\__|_|\___/|_| |_|___/
//
#define MQT_SUB_PAT "test/#"

typedef struct {
  char topic[256];
  int run;
  struct mosquitto_message *msg;
} userdata_t;

//             _ _ _                _        
//    ___ __ _| | | |__   __ _  ___| | _____ 
//   / __/ _` | | | '_ \ / _` |/ __| |/ / __|
//  | (_| (_| | | | |_) | (_| | (__|   <\__ \
//   \___\__,_|_|_|_.__/ \__,_|\___|_|\_\___/
// 
void on_connect(struct mosquitto *mqt, void *obj, int rc) {
  userdata_t *ud = obj;
  printf("-> Connected\n");
  // subscribe
  if ((mosquitto_subscribe(mqt, NULL, ud->topic, 1)) != MOSQ_ERR_SUCCESS) {
    perror("Could not suscribe");
    exit(4);
  }
}

void on_disconnect(struct mosquitto *mqt, void *obj, int rc) {
  printf("<- Disconnected\n");
}

void on_subscribe(struct mosquitto *mqt, void *obj, int mid, int qos_count, const int *grated_qos) {
  printf("-> Subscribed\n");
}

void on_unsubscribe(struct mosquitto *mqt, void *obj, int mid) {
  printf("<- Unsubscribed\n");
}

void on_message(struct mosquitto *mqt, void *obj, const struct mosquitto_message *msg) {
  userdata_t *ud = obj;
  printf("<- message: %s\n", (char *)msg->payload);
  printf("   topic: %s\n", msg->topic);
  if (strcmp((char *)msg->payload, "stop") == 0) {
    ud->run = 0;
  }
  mosquitto_message_copy(ud->msg, msg);
}


//                   _       
//   _ __ ___   __ _(_)_ __  
//  | '_ ` _ \ / _` | | '_ \ 
//  | | | | | | (_| | | | | |
//  |_| |_| |_|\__,_|_|_| |_|
//
int main(int argc, char const *argv[]) {
  struct mosquitto *mqt;
  userdata_t ud = {.topic = MQT_SUB_PAT, .run = 1};

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
  if ((mosquitto_connect(mqt, "localhost", 1883, 10)) != MOSQ_ERR_SUCCESS) {
    perror("Could not connect to broker");
    exit(3);
  }
  
  // main loop
  while (ud.run) {
    if (mosquitto_loop(mqt, 0, 1) != MOSQ_ERR_SUCCESS) {
      perror("mosquitto loop error");
      break;
    }
    usleep(10E3);
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
