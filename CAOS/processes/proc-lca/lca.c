#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lca.h"

#define MAX_BUFF_SIZE 256

int get_parent(pid_t pid, pid_t* result) {
  if (pid == 0 || pid == 1) {
    *result = 0;
    return 0;
  }

  char buf[MAX_BUFF_SIZE];
  sprintf(buf, "/proc/%d/stat", pid);

  FILE* file;
  file = fopen(buf, "r");

  if (file == NULL) {
    return -1;
  }

  memset(buf, '\0', MAX_BUFF_SIZE);

  char* fget = fgets(buf, MAX_BUFF_SIZE, file);
  if (fget == NULL) {
    return -1;
  }

  buf[MAX_BUFF_SIZE - 1] = '\0';
  fclose(file);

  char res[MAX_BUFF_SIZE];
  memset(res, '\0', MAX_BUFF_SIZE);

  int counter = 0;
  int cur = 0;
  for (uint64_t i = 0; i < MAX_BUFF_SIZE; ++i) {
    if (buf[i] == ' ') {
      counter += 1;
    }

    if (counter == 4) {
      *result = atoi(res);
      return 0;
    }

    if (counter == 3) {
      res[cur] = buf[i];
      cur += 1;
    }
  }

  *result = 0;
  return 0;
}

pid_t find_lca(pid_t x, pid_t y) {
  if (x == 0 || y == 0) {
    return x;
  }

  while (x != 0 || y != 0) {
    pid_t res;
    if (x > y) {
      if (get_parent(x, &res) < 0) {
        return -1;
      }

      x = res;
    } else if (y > x) {
      if (get_parent(y, &res) < 0) {
        return -1;
      }

      y = res;
    } else {
      return x;
    }
  }

  return -1;
}