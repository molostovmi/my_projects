#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

enum {
  MAX_ARGS_COUNT = 256,
  MAX_CHAIN_LINKS_COUNT = 256,
  MAX_BUFF_SIZE = 256
};

typedef struct {
  char* command;
  uint64_t argc;
  char* argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
  uint64_t chain_links_count;
  chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

void parse(char* command, chain_link_t* chain_link, int start, int end) {
  char buf[MAX_BUFF_SIZE];
  int cur = 0;
  int argc = 0;

  for (int i = start; i < end; ++i) {
    if (command[i] != ' ') {
      buf[cur] = command[i];
      ++cur;
    } else {
      buf[cur] = '\0';

      chain_link->argv[argc] = (char*)malloc(strlen(buf));
      if (chain_link->argv[argc] == NULL) {
        return;
      }

      memcpy(chain_link->argv[argc], buf, strlen(buf));
      memset(buf, '\0', sizeof(buf));

      cur = 0;
      ++argc;
    }
  }

  if (buf[0] != '\0') {
    buf[cur] = '\0';

    chain_link->argv[argc] = (char*)malloc(strlen(buf));
    if (chain_link->argv[argc] == NULL) {
      return;
    }

    memcpy(chain_link->argv[argc], buf, strlen(buf));
    memset(buf, '\0', sizeof(buf));
    ++argc;
  }

  chain_link->command = chain_link->argv[0];
  chain_link->argc = argc;
}

void create(char* command, chain_t* chain) {
  int size = 0;
  int cur = 0;
  bool is_update = true;

  int i;
  for (i = 0; i < strlen(command); ++i) {
    if (!is_update && command[i] != ' ') {
      is_update = true;
      cur = i;
    } else if (command[i] == '|') {
      parse(command, &chain->chain_links[size], cur, i);
      is_update = false;
      ++size;
    }
  }

  parse(command, &chain->chain_links[size], cur, i);
  chain->chain_links_count = size + 1;
}

void run(chain_t* chain) {
  int last = 0;

  for (int i = 0; i < chain->chain_links_count; ++i) {
    int pipe_fd[2];
    if ((i != chain->chain_links_count - 1) && (pipe(pipe_fd) < 0)) {
      return;
    }

    pid_t pid = fork();
    if (pid < 0) {
      return;
    } else if (pid > 0) {
      if (i != 0) {
        close(last);
      }
      if (i != chain->chain_links_count - 1) {
        close(pipe_fd[1]);
      }

      last = pipe_fd[0];
    } else {
      if (i != 0) {
        dup2(last, STDIN_FILENO);
      }
      if (i != chain->chain_links_count - 1) {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
      }

      chain->chain_links[i].argv[chain->chain_links[i].argc] = NULL;
      execvp(chain->chain_links[i].command, chain->chain_links[i].argv);
      perror("execvp");
    }
  }
}

int main(int argc, char* argv[]) {
  chain_t chain;
  create(argv[1], &chain);
  run(&chain);
  return 0;
}