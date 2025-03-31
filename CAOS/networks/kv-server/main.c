#include <linux/limits.h>

#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 2048

typedef struct StorageItem{
  char key[PATH_MAX];
  char value[PATH_MAX];
  struct StorageItem* next;
} StorageItem;

typedef struct Storage{
  struct StorageItem* head;
} Storage;

StorageItem* find(Storage* storage, char* key) {
  StorageItem* current = storage->head;
  while (current != NULL) {
    if (strncmp(current->key, key, PATH_MAX) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void set(Storage* storage, char* key, char* value) {
  StorageItem* element = find(storage, key);
  if (element == NULL) {
    element = malloc(sizeof(StorageItem));
    strncpy(element->key, key, PATH_MAX - 1);
    element->next = storage->head;
    storage->head = element;
  }
  strncpy(element->value, value, PATH_MAX - 1);
}

char* get(Storage* storage, char* key) {
  StorageItem* element = find(storage, key);
  if (element == NULL) {
    return "";
  } else {
    return element->value;
  }
}

typedef struct server_args{
  Storage* storage;
  int fd;
} server_args_t;

void Clear(server_args_t* args) {
  close(args->fd);
  free(args);
}

void* ClientRuntime(void* args) {
  server_args_t* client_args = (server_args_t*)args;
  char buffer[MAX_SIZE];

  int cnt;
  while ((cnt = read(client_args->fd, buffer, MAX_SIZE - 1)) > 0) {
    buffer[cnt] = '\0';
    char request[MAX_SIZE];
    char key[MAX_SIZE];
    char value[MAX_SIZE];

    int i = 0;
    while (buffer[i] != ' ' && buffer[i] != '\0') {
      request[i] = buffer[i];
      ++i;
    }

    request[i] = '\0';
    ++i;

    int j = 0;
    while (buffer[i] != '\0' && buffer[i] != ' ') {
      key[j] = buffer[i];
      ++i;
      ++j;
    }

    key[j] = '\0';
    ++i;

    if (strcmp(request, "set") == 0) {
      j = 0;
      while (buffer[i] != '\0' && buffer[i] != '\0') {
        value[j] = buffer[i];
        ++i;
        ++j;
      }

      value[j] = '\0';
      set(client_args->storage, key, value);
    } else {
      char* value = get(client_args->storage, key);

      char* new_value = (char*)malloc(strlen(value) + 1);
      strcpy(new_value, value);
      new_value[strlen(value)] = '\n';

      if (send(client_args->fd, new_value, strlen(new_value), 0) < 0) {
        Clear(client_args);
        return NULL;
      }
    }
  }

  Clear(client_args);
  pthread_exit(NULL);
  return NULL;
}

int ServerRuntime(server_args_t* args) {
  int client_fd = accept(args->fd, NULL, NULL);
  if (client_fd < 0) {
    return -1;
  }

  server_args_t* thread_args = (server_args_t*)malloc(sizeof(server_args_t));
  thread_args->storage = args->storage;
  thread_args->fd = client_fd;

  pthread_t client_phtread;
  if (pthread_create(&client_phtread, NULL, ClientRuntime, (void*)thread_args) < 0) {
    return -1;
  }

  pthread_detach(client_phtread);
  return 0;
}

int main(int argc, char* argv[]) {
  Storage* storage = malloc(sizeof(Storage));
  storage->head = NULL;
  int server_port = atoi(argv[1]);

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    return -1;
  }

  struct sockaddr_in socket_ipv4_address = {.sin_family = AF_INET,
                                            .sin_addr = {inet_addr("127.0.0.1")},
                                            .sin_port = htons(server_port)};

  int bind_result = bind(socket_fd, (struct sockaddr*)&socket_ipv4_address, sizeof(socket_ipv4_address));
  if (bind_result < 0) {
    return -1;
  }

  int listen_result = listen(socket_fd, SOMAXCONN);
  if (listen_result < 0) {
    return -1;
  }

  server_args_t* args = (server_args_t*)malloc(sizeof(server_args_t));
  args->storage = storage;
  args->fd = socket_fd;

  while (true) {
    if (ServerRuntime(args) < 0) {
      shutdown(socket_fd, SHUT_RDWR);
      close(socket_fd);
      Clear(args);
      return -1;
    }
  }

  Clear(args);
  return 0;
}