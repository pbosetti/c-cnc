//   _____ _                    _   _                     ____  
//  |_   _(_)_ __ ___   ___  __| | | | ___   ___  _ __   |___ \
//    | | | | '_ ` _ \ / _ \/ _` | | |/ _ \ / _ \| '_ \    __) |
//    | | | | | | | | |  __/ (_| | | | (_) | (_) | |_) |  / __/ 
//    |_| |_|_| |_| |_|\___|\__,_| |_|\___/ \___/| .__/  |_____|
//                                               |_|            
// This version uses a SIGALRM signal, raised by the kernel at regular 
// intervals. A call to sleep() returns after the given time has elapsed
// or after receiving an ALRM signal, whichever first.
// This version does not need any tuning of the step time and it is typically
// the best way to accomplish the task. On Realtime kernels the time jitter 
// can be about +-1 us, on normal kernels about +-1 ms
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// Empty signal handler
void handler(void) {};

int main(int argc, const char *argv[]) {
  int i=0;
  double t0=0, t, dt;
  struct timespec ts;
  struct itimerval itv;

  // Configure our time
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = 5000; // 5 ms
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = 5000;

  // Install the signal handler for SIGALRM
  signal(SIGALRM, handler);

  // Set the timer
  if(setitimer(ITIMER_REAL, &itv, NULL)){
    perror("I was not able to set the timer!");
  };

  // Csv header
  fprintf(stdout, "n, dt\n");

  clock_gettime(CLOCK_MONOTONIC, &ts);
  t0 = ts.tv_sec + ts.tv_nsec / 1.0E9;

  for(i=0; i<1000; i++) {
    // the sleep waits for the next SIGALRM, but no more than 1 s
    sleep(1);

    // Get the time
    clock_gettime(CLOCK_MONOTONIC, &ts);
    t = ts.tv_sec + ts.tv_nsec / 1.0E9;
    dt = t - t0;
    t0 = t;

    // Print time
    fprintf(stdout, "%d, %f\n", i, dt);
  }
  
}