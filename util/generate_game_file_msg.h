#ifndef GENERATE_GAME_FILE_MSG_H
#define GENERATE_GAME_FILE_MSG_H

#include "prize.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const char *generate_invalid_msg(size_t idx) {
  char *game_msg = (char *)malloc(sizeof(char) * 4096);
  sprintf(game_msg, "[#%zu] Invalid line detected.\n", idx);
  return (const char *)game_msg;
}

static const char *generate_game_file_msg(size_t idx, uint64_t *user_numbers,
                                          uint64_t *winning_numbers) {
  enum Prize prize = calculate_prize(user_numbers, winning_numbers);
  char *game_msg = (char *)malloc(sizeof(char) * 4096 * 2);
  sprintf(game_msg,
          "[#%zu]    Your numbers: %2llu %2llu %2llu %2llu %2llu "
          "%2llu\n"
          "[#%zu] Winning numbers: %2llu %2llu %2llu %2llu %2llu "
          "%2llu\n"
          "[#%zu]           Prize: %s (%lld won)\n",
          idx, user_numbers[0], user_numbers[1], user_numbers[2],
          user_numbers[3], user_numbers[4], user_numbers[5], idx,
          winning_numbers[0], winning_numbers[1], winning_numbers[2],
          winning_numbers[3], winning_numbers[4], winning_numbers[5], idx,
          prize2string(prize), balance(prize));
  return (const char *)game_msg;
}

#endif