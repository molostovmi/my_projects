#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

bool is_same_file(const char* lhs_path, const char* rhs_path) {
  struct stat first_file;
  struct stat second_file;

  if (stat(lhs_path, &first_file) < 0) {
    return false;
  }

  if (stat(rhs_path, &second_file) < 0) {
    return false;
  }

  return first_file.st_ino == second_file.st_ino;
}

int main(int argc, const char* argv[]) {
  if (argc != 3) {
    return 1;
  }

  if (is_same_file(argv[1], argv[2])) {
    printf("yes");
  } else {
    printf("no");
  }

  return 0;
}