#ifndef GENERATE_NUMBERS_H
#define GENERATE_NUMBERS_H

#include "../chacha20/chacha20.h"
#include <stdbool.h>
#include <stdint.h>

static inline uint64_t *generate_numbers() {
  uint64_t *res = malloc(sizeof(uint64_t) * 6);
  for (int i = 0; i < 6; i++) {
    uint64_t num;
    bool is_not_match = true;
    do {
      num = (chacha20_random64() % 45) + 1;
      is_not_match = true;
      for (int j = 0; j < i; j++) {
        is_not_match &= (res[j] != num);
      }
    } while (!is_not_match);
    res[i] = num;
  }
  return res;
}

#endif