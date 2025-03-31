#include <dirent.h>
#include <fcntl.h>
#include <pcre.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct stat stat_t;
typedef struct dirent dirent_t;

#define MAX_SIZE 2048

int len_of_str(char* file, stat_t* file_info, int count_of_bytes) {
  int size_of_string = 0;
  while (count_of_bytes + size_of_string < file_info->st_size &&
         file[count_of_bytes + size_of_string] != '\n') {
    ++size_of_string;
  }

  return size_of_string;
}

int grep(const char* filename, stat_t* file_info, pcre* regular) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    return -1;
  }

  char* file = (char*)mmap(NULL, file_info->st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (file == MAP_FAILED) {
    close(fd);
    return -1;
  }

  int count_of_bytes = 0;
  int number_of_line = 1;
  while (count_of_bytes < file_info->st_size) {
    if (file[count_of_bytes] == '\n') {
      ++count_of_bytes;
      ++number_of_line;
      continue;
    }

    int size_of_string = len_of_str(file, file_info, count_of_bytes);
    int ovector[30];
    if (pcre_exec(regular, NULL, file + count_of_bytes, size_of_string, 0, 0, ovector, 30) > 0) {
      char str[MAX_SIZE];
      memcpy(str, file + count_of_bytes, size_of_string);
      str[size_of_string] = '\0';

      printf("%s:%i: %s\n", filename, number_of_line, str);
    }

    count_of_bytes += size_of_string + 1;
    ++number_of_line;
  }

  close(fd);

  int unmap_file = munmap(file, file_info->st_size);
  return unmap_file;
}

int grep_directory(const char* name, stat_t* file_info, pcre* regular) {
  if (!S_ISDIR(file_info->st_mode) || S_ISLNK(file_info->st_mode)) {
    int grep_file = grep(name, file_info, regular);

    if (grep_file == -1) {
      return -1;
    }
    return 0;
  }

  DIR* directory = opendir(name);
  if (directory == NULL) {
    return -1;
  }

  dirent_t* directory_info;
  while ((directory_info = readdir(directory)) != NULL) {
    uint64_t len_of_child = strlen(directory_info->d_name);
    if (len_of_child == 1 && directory_info->d_name[0] == '.') {
      continue;
    }

    if (strcmp(directory_info->d_name, "..") == 0) {
      continue;
    }

    uint64_t len_of_new_name = strlen(name) + len_of_child + 2;
    char new_name[MAX_SIZE];

    snprintf(new_name, len_of_new_name, "%s/%s", name, directory_info->d_name);

    stat_t new_file_stat;
    if (stat(new_name, &new_file_stat) == -1) {
      closedir(directory);
      return -1;
    }

    if (grep_directory(new_name, &new_file_stat, regular) == -1) {
      closedir(directory);
      return -1;
    }
  }

  closedir(directory);
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    return -1;
  }

  pcre* regular;
  const char* error;
  int erroroffset;
  regular = pcre_compile(argv[1], 0, &error, &erroroffset, NULL);

  if (regular == NULL) {
    fprintf(stderr, "Regular expression is failed %d: %s", erroroffset, error);
    return 1;
  }

  stat_t file_info;
  if (stat(argv[2], &file_info) == -1) {
    pcre_free(regular);
    return 1;
  }

  if (grep_directory(argv[2], &file_info, regular) == -1) {
    pcre_free(regular);
    return 1;
  }

  pcre_free(regular);
  return 0;
}