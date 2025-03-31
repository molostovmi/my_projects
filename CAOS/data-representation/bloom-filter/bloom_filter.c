#include <stdlib.h>

#include "bloom_filter.h"

const uint64_t MODULE = 1000000007;

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed) {
  uint64_t ans = (uint64_t)(str[0] + 1 - 'a');
  int i = 1;

  while (str[i] != '\0') {
    ans = ((ans * seed) % modulus + (uint64_t)(str[i] + 1 - 'a')) % modulus;
    ++i;
  }

  return ans;
}

void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size, hash_fn_t hash_fn, uint64_t hash_fn_count) {
  bloom_filter->set = (uint64_t*)malloc(sizeof(uint64_t) * (set_size / 64 + (set_size % 64 != 0)));

  if (bloom_filter->set == NULL) {
    bloom_filter->set_size = 0;
    return;
  }

  bloom_filter->set_size = set_size;
  for (uint64_t i = 0; i < bloom_filter->set_size / 64 + (bloom_filter->set_size % 64 != 0); ++i) {
    bloom_filter->set[i] = 0;
  }

  bloom_filter->hash_fn = hash_fn;
  bloom_filter->hash_fn_count = hash_fn_count;
}

void bloom_destroy(struct BloomFilter* bloom_filter) {
  free(bloom_filter->set);
  bloom_filter->set = NULL;
  bloom_filter->set_size = 0;
}

void bloom_insert(struct BloomFilter* bloom_filter, Key key) {
  uint64_t* hash_f_values = (uint64_t*)malloc(sizeof(uint64_t) * bloom_filter->hash_fn_count);
  uint64_t cur_size = bloom_filter->set_size / 64 + (bloom_filter->set_size % 64 != 0);

  if (hash_f_values == NULL) {
    return;
  }

  for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
    hash_f_values[i] = bloom_filter->hash_fn(key, MODULE, i) % (cur_size * 64);
  }

  for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
    bloom_filter->set[hash_f_values[i] / 64] |= ((uint64_t)1 << (hash_f_values[i] % 64));
  }

  free(hash_f_values);
}

bool bloom_check(struct BloomFilter* bloom_filter, Key key) {
  uint64_t is_check = 1;
  uint64_t cur_size = bloom_filter->set_size / 64 + (bloom_filter->set_size % 64 != 0);

  for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
    uint64_t hash = bloom_filter->hash_fn(key, MODULE, i) % (cur_size * 64);
    is_check &= (bool)((bloom_filter->set[hash / 64] >> (hash % 64)) & 1);
  }

  return (bool)is_check;
}