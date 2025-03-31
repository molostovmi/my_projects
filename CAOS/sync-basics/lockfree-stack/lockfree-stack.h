#pragma once

#include <stdint.h>
#include <stdatomic.h>

struct node {
  uintptr_t value;
  struct node* prev;
};

typedef struct lfstack {
  _Atomic(struct node*) head;
} lfstack_t;

int lfstack_init(lfstack_t *stack) {
  atomic_init(&stack->head, NULL);
  return 0; // success
}

int lfstack_push(lfstack_t *stack, uintptr_t value) {
  struct node* new_node = (struct node*)malloc(sizeof(struct node));
  new_node->value = value;

  struct node* origin;
  do {
    origin = stack->head;
    new_node->prev = origin;
  } while (!atomic_compare_exchange_weak(&stack->head, &origin, new_node));

  return 0; // success
}

int lfstack_pop(lfstack_t *stack, uintptr_t *value) {
  struct node* origin;
  struct node* new_node;
  do {
    if (stack->head == NULL) {
      *value = 0;
      return 0;
    }

    origin = stack->head;
    new_node = origin->prev;
  } while (!atomic_compare_exchange_weak(&stack->head, &origin, new_node));

  *value = origin->value;
  free(origin);
  return 0; // success
}

int lfstack_destroy(lfstack_t *stack) {
  uintptr_t ptr;
  lfstack_pop(stack, &ptr);

  while (ptr != (uintptr_t)NULL) {
    lfstack_pop(stack, &ptr);
  }

  return 0; // success
}