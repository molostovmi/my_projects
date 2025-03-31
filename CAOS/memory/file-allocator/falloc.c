#include "falloc.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

void falloc_init(file_allocator_t* allocator, const char* filepath, uint64_t allowed_page_count) {
  int cur = allowed_page_count * PAGE_SIZE;
  int fd = open(filepath, O_RDWR | O_CREAT | O_EXCL, 0777);

  if (fd == -1 && (errno == EEXIST)) {
    fd = open(filepath, O_RDWR, 0777);
    if (fd == -1) {
      return;
    }

    struct stat file_info;
    if (fstat(fd, &file_info) < 0) {
      return;
    }

    allocator->curr_page_count = (file_info.st_size / PAGE_SIZE) +
                                 (uint64_t)(file_info.st_size % PAGE_SIZE != 0 ? 1 : 0) - 1;
  } else if (fd != -1) {
    cur += PAGE_SIZE;

    if (ftruncate(fd, cur) < 0) {
      return;
    }
    allocator->curr_page_count = 0;
  } else {
    return;
  }

  void* data = mmap(NULL, cur, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    return;
  }

  allocator->fd = fd;
  allocator->base_addr = data + PAGE_SIZE;
  allocator->page_mask = (uint64_t*)data;
  allocator->allowed_page_count = allowed_page_count;
}

void falloc_destroy(file_allocator_t* allocator) {
  if (munmap((void*)allocator->page_mask, allocator->allowed_page_count * PAGE_SIZE) == -1) {
    return;
  }
  close(allocator->fd);

  allocator->base_addr = NULL;
  allocator->page_mask = NULL;
  allocator->curr_page_count = 0;
  allocator->allowed_page_count = 0;
}

void* falloc_acquire_page(file_allocator_t* allocator) {
  union PageMask {
    uint64_t* a;
    char* b;
  };
  union PageMask page_mask = {allocator->page_mask};

  for (int i = 0; i < allocator->allowed_page_count; ++i) {
    if (page_mask.b[i] == 0) {
      page_mask.b[i] = 1;
      ++allocator->curr_page_count;
      return allocator->base_addr + i * PAGE_SIZE;
    }
  }

  return NULL;
}

void falloc_release_page(file_allocator_t* allocator, void** addr) {
  union PageMask {
    uint64_t* a;
    char* b;
  };
  union PageMask page_mask = {allocator->page_mask};
  page_mask.b[(*addr - allocator->base_addr) / PAGE_SIZE] = 0;

  allocator->page_mask = page_mask.a;
  allocator->curr_page_count -= 1;
  *addr = NULL;
}