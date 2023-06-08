// SPDX-License-Identifier: MIT

#ifndef CHECK_DUPLICATE_H
#define CHECK_DUPLICATE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static inline bool check_duplicate(uint64_t *numbers) {
  bool is_not_duplicate = true;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < i; j++) {
      is_not_duplicate &= (numbers[i] != numbers[j]);
    }
  }
  return !is_not_duplicate;
}

#endif