#ifndef PRIZE_H
#define PRIZE_H

#include <stdbool.h>
#include <stdint.h>

enum Prize {
  NO_PRIZE,
  FIRST_PRIZE,
  SECOND_PRIZE,
  THIRD_PRIZE,
  FORTH_PRIZE,
};

static inline uint64_t balance(enum Prize prize) {
  switch (prize) {
  case FIRST_PRIZE:
    return 2000000000;
    break;
  case SECOND_PRIZE:
    return 2000000;
    break;
  case THIRD_PRIZE:
    return 50000;
    break;
  case FORTH_PRIZE:
    return 5000;
    break;
  default:
    break;
  }
  return 0;
}

static inline const char *prize2string(enum Prize prize) {
  switch (prize) {
  case FIRST_PRIZE:
    return "first prize";
    break;
  case SECOND_PRIZE:
    return "second prize";
    break;
  case THIRD_PRIZE:
    return "third prize";
    break;
  case FORTH_PRIZE:
    return "forth prize";
    break;
  case NO_PRIZE:
    return "no prize";
    break;
  default:
    break;
  }
  return "";
}

static inline enum Prize calculate_prize(uint64_t *user_numbers,
                                         uint64_t *winning_numbers) {
  int cnt = 0;
  bool fill[45];
  for (int i = 0; i < 6; i++)
    fill[user_numbers[i] - 1] = true;
  for (int i = 0; i < 6; i++)
    if (fill[winning_numbers[i] - 1])
      cnt++;
  switch (cnt) {
  case 6:
    return FIRST_PRIZE;
    break;
  case 5:
    return SECOND_PRIZE;
    break;
  case 4:
    return THIRD_PRIZE;
    break;
  case 3:
    return FORTH_PRIZE;
    break;
  default:
    break;
  }
  return NO_PRIZE;
}

#endif