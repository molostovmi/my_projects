#include <linux/limits.h>

#include <asm/unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct user_regs_struct user_regs_struct_t;

typedef struct Counter{
  char filename[PATH_MAX];
  int counter;
  struct Counter* next;
} Counter;

typedef struct Counters{
  struct Counter* head;
} Counters;

void increment(Counters* counters, char* filename, int value) {
  Counter* current = counters->head;
  while (current != NULL) {
    if (strncmp(current->filename, filename, PATH_MAX) == 0) {
      current->counter += value;
      return;
    }
    current = current->next;
  }
  Counter* new_head = malloc(sizeof(Counter));
  new_head->next = counters->head;
  new_head->counter = value;
  strncpy(new_head->filename, filename, PATH_MAX - 1);
  counters->head = new_head;
}

void print(Counters* counters) {
  Counter* current = counters->head;
  while (current != NULL) {
    printf("%s:%d\n", current->filename, current->counter);
    current = current->next;
  }
}

int main(int argc, char *argv[]) {
  Counters* counters = malloc(sizeof(Counter));

  if (counters == NULL) {
    return -1;
  }
  counters->head = NULL;

  pid_t pid = fork();
  if (pid < 0) {
    return -1;
  } else if (pid != 0) {
    int status;

    while (waitpid(pid, &status, 0) && !WIFEXITED(status)) {
      user_regs_struct_t state;
      ptrace(PTRACE_GETREGS, pid, 0, &state);

      if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP &&
          state.orig_rax == __NR_write) {
        char file_name[PATH_MAX];
        memset(file_name, '\0', PATH_MAX);

        char buf[PATH_MAX];
        memset(buf, '\0', PATH_MAX);
        sprintf(buf, "/proc/%d/fd/%d", pid, (int)state.rdi);

        if (readlink(buf, file_name, PATH_MAX - 1) < 0) {
          return -1;
        }

        ptrace(PTRACE_SYSCALL, pid, 0, 0);
        waitpid(pid, &status, 0);
        ptrace(PTRACE_GETREGS, pid, 0, &state);

        increment(counters, file_name, state.rax);
      }
      ptrace(PTRACE_SYSCALL, pid, 0, 0);
    }
    print(counters);
  } else {
    char* args[15];
    for (int i = 0; i < argc - 1; ++i) {
      args[i] = argv[i + 1];
    }
    args[argc - 1] = NULL;

    ptrace(PTRACE_TRACEME, 0, 0, 0);
    execvp(argv[1], args);
    perror("execl");
  }

  return 0;
}