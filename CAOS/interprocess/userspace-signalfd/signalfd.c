#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int pipefd[2];

void Handler(int number_of_signal) {
  if (write(pipefd[1], &number_of_signal, sizeof(int)) < 0) {
    perror("Handler not correct work");
  }
}

int signalfd() {
  if (pipe(pipefd) == -1) {
    exit(EXIT_FAILURE);
  }

  struct sigaction sigact;
  sigact.sa_handler = Handler;
  sigact.sa_flags = 0;
  sigemptyset(&sigact.sa_mask);

  for (int i = 1; i < 32; i++) {
    if (i != SIGKILL && i != SIGSTOP) {
      if (sigaction(i, &sigact, NULL) == -1) {
        exit(EXIT_FAILURE);
      }
    }
  }

  return pipefd[0];
}