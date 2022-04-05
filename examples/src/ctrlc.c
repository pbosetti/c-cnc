#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sigjmp_buf JumpBuffer;

void sig_int(int signal) {
  siglongjmp(JumpBuffer, 1);
}

void sig_info(int signal) {
  siglongjmp(JumpBuffer, 2);
}

int main(int argc, char const *argv[]) {
  int running = 1, i = 0;
  signal(SIGINT, sig_int);
  signal(SIGINFO, sig_info);
  int r = sigsetjmp(JumpBuffer, 1);

  if (r == 1) {
    running = 0;
  } 
  else if (r == 2) {
    i = 0;
  }

  while (running) {
    printf("%d\n", i++);
    sleep(1);
  }

  printf("Exiting\n");

  return 0;
}
