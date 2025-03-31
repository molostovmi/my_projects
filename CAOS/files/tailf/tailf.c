#include <fcntl.h>
#include <unistd.h>

enum {
  STDOUT = 1,
  SLEEP_TIME = 100,
  BUF_SIZE = 256
};

int main(int argc, char** argv) {
  if (argc != 2) {
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);

  for(;;) {
    char buf[BUF_SIZE];
    int len = read(fd, buf, BUF_SIZE);

    if (len < 0) {
      break;
    }

    write(STDOUT, buf, len);
    usleep(SLEEP_TIME);
  }

  close(fd);

  return 0;
}