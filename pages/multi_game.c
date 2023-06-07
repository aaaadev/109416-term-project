// SPDX-License-Identifier: MIT

#include "multi_game.h"
#include "../console/console.h"
#include "../console/input.h"
#include "../console/page.h"
#include "../constants.h"
#include "../util/check_duplicate.h"
#include "../util/generate_game_file_msg.h"
#include "../util/generate_numbers.h"
#include "../util/prize.h"
#include "../util/split_whitespace.h"
#include "multi_game_result.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// PRIVATE
static enum ConsoleResult draw_view(struct ConsoleCtx *ctx, size_t cur,
                                    const char *filename,
                                    const char *number_of_games,
                                    enum FileErrorMulti err) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  char *msg = malloc(sizeof(char) *
                     (strlen(MSG_MULTI) + strlen(MSG_MULTI2) +
                      strlen(MSG_ALREADY_EXISTS_MULTI) +
                      strlen(MSG_NOT_VALID_FILE_NAME_MULTI) +
                      strlen(MSG_NOT_VALID_NUMBER_OF_GAMES_MULTI) + 4096 +
                      strlen(filename) + 4096 + strlen(number_of_games) + 1));
  msg[0] = '\0';
  strcat(msg, MSG_MULTI);
  if ((err & ALREADY_EXISTS_FILE) != 0) {
    strcat(msg, MSG_ALREADY_EXISTS_MULTI);
  }
  if ((err & NOT_VALID_FILE_NAME) != 0) {
    strcat(msg, MSG_NOT_VALID_FILE_NAME_MULTI);
  }
  if ((err & NOT_VALID_NUMBER_OF_GAMES) != 0) {
    strcat(msg, MSG_NOT_VALID_NUMBER_OF_GAMES_MULTI);
  }
  if ((err & ERROR_FILE_OPEN) != 0) {
    strcat(msg, MSG_ERROR_FILE_OPEN_MULTI);
  }
  strcat(msg, MSG_MULTI2);
  if (cur == 0) {
    strcat(msg, "> filename: ");
    strcat(msg, filename);
    strcat(msg, "\nnumber of games : ");
    strcat(msg, number_of_games);
    strcat(msg, "\nok");
    strcat(msg, "\nback");
  } else if (cur == 1) {
    strcat(msg, "filename: ");
    strcat(msg, filename);
    strcat(msg, "\n> number of games : ");
    strcat(msg, number_of_games);
    strcat(msg, "\nok");
    strcat(msg, "\nback");
  } else if (cur == 2) {
    strcat(msg, "filename: ");
    strcat(msg, filename);
    strcat(msg, "\nnumber of games : ");
    strcat(msg, number_of_games);
    strcat(msg, "\n> ok");
    strcat(msg, "\nback");
  } else if (cur == 3) {
    strcat(msg, "filename: ");
    strcat(msg, filename);
    strcat(msg, "\nnumber of games : ");
    strcat(msg, number_of_games);
    strcat(msg, "\nok");
    strcat(msg, "\n> back");
  }
  update_result(&res,
                simple_textvh(ctx, msg, HORIZONTAL_CENTER, VERTICAL_CENTER));
  free(msg);
  return res;
}

// PRIVATE
enum ConsoleResult handle_key_multi_game(const char *text, void *args,
                                         enum InputControll *cont) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  char c = text[0];
  struct HandleKeyDataMulti *data = (struct HandleKeyDataMUlti *)args;
  char *filename = data->filename;
  char *number_of_games = data->number_of_games;
  size_t filename_len = strlen(filename);
  size_t number_of_games_len = strlen(number_of_games);
  if (c == '\033') {
    read(STDIN_FILENO, &c, 1);
    if (c == '[') {
      read(STDIN_FILENO, &c, 1);
      switch (c) {
      case 'A':
        // UP
        if (data->cur != 0) {
          data->cur--;
        }
        break;
      case 'B':
        // DOWN
        if (data->cur != 3) {
          data->cur++;
        }
        break;
      case 'C':
        // RIGHT
        break;
      case 'D':
        // LEFT
        break;
      default:
        break;
      }
      update_result(&res, draw_view(data->page_ctx->ctx, data->cur, filename,
                                    number_of_games, NOTHING_MULTI));
      *cont = INPUTC_CONTINUE;
    }

  } else {
    if (c == '\n') {
      if (data->cur == 2) {
        FILE *file = fopen(filename, "r");
        int err = 0;
        if (file) {
          err |= ALREADY_EXISTS_FILE;
          fclose(file);
        }
        if (filename_len == 0) {
          err |= NOT_VALID_FILE_NAME;
        }
        size_t games = 0;
        if ((sscanf(number_of_games, "%llu", &games) != 1) ||
            !(MIN_GAMES <= games && games <= MAX_GAMES)) {
          err |= NOT_VALID_NUMBER_OF_GAMES;
        }
        if (err != 0) {
          update_result(&res, draw_view(data->page_ctx->ctx, data->cur,
                                        filename, number_of_games, err));
          *cont = INPUTC_CONTINUE;
        } else {
          file = fopen(filename, "w");
          if (!file) {
            err |= ERROR_FILE_OPEN;
          }
          if (err != 0) {
            update_result(&res, draw_view(data->page_ctx->ctx, data->cur,
                                          filename, number_of_games, err));
            *cont = INPUTC_CONTINUE;
          } else {
            size_t cnt_first = 0;
            int64_t first_earned = 0;
            size_t cnt_second = 0;
            int64_t second_earned = 0;
            size_t cnt_third = 0;
            int64_t third_earned = 0;
            size_t cnt_forth = 0;
            int64_t forth_earned = 0;
            size_t cnt_no = 0;
            int64_t total_earned = 0;
            int64_t total_spent = 0;
            for (size_t i = 0; i < games; i++) {
              uint64_t *user_numbers = generate_numbers();
              uint64_t *winning_numbers = generate_numbers();
              enum Prize prize = calculate_prize(user_numbers, winning_numbers);
              int64_t bal = balance(prize);
              const char *prize_str = prize2string(prize);
              const char *game_msg =
                  generate_game_file_msg(i + 1, user_numbers, winning_numbers);
              fputs(game_msg, file);
              free(game_msg);
              total_spent += (int64_t)GAME_PRICE;
              total_earned += (int64_t)bal;
              switch (prize) {
              case FIRST_PRIZE:
                cnt_first++;
                first_earned += (int64_t)bal;
                break;
              case SECOND_PRIZE:
                cnt_second++;
                second_earned += (int64_t)bal;
                break;
              case THIRD_PRIZE:
                cnt_third++;
                third_earned += (int64_t)bal;
                break;
              case FORTH_PRIZE:
                cnt_forth++;
                forth_earned += (int64_t)bal;
                break;
              case NO_PRIZE:
                cnt_no++;
                break;
              default:
                break;
              }
              free(user_numbers);
              free(winning_numbers);
            }
            char *game_statmsg = malloc(sizeof(char) * 4096 * 2);
            sprintf(game_statmsg,
                    "[statistics]  First prize: %zu (%lld won)\n"
                    "[statistics] Second prize: %zu (%lld won)\n"
                    "[statistics]  Third prize: %zu (%lld won)\n"
                    "[statistics]  Forth prize: %zu (%lld won)\n"
                    "[statistics]     No prize: %zu (%lld won)\n"
                    "[statistics] Total spent: %lld\n"
                    "[statistics] Total earned: %lld\n"
                    "[statistics] Total margin (earned - spent): %lld\n",
                    cnt_first, first_earned, cnt_second, second_earned,
                    cnt_third, third_earned, cnt_forth, forth_earned, cnt_no,
                    0ll, total_spent, total_earned, total_earned - total_spent);
            free(game_statmsg);
            fputs(game_statmsg, file);
            fflush(file);
            fclose(file);
            size_t idx =
                add_page(data->page_ctx, on_view_multi_game_result, true);
            struct ResultArgsMulti *res_args =
                malloc(sizeof(struct ResultArgsMulti));
            res_args->page_ctx = data->page_ctx;
            res_args->first_prize = cnt_first;
            res_args->second_prize = cnt_second;
            res_args->third_prize = cnt_third;
            res_args->forth_prize = cnt_forth;
            res_args->no_prize = cnt_no;
            res_args->total_earned = total_earned;
            res_args->total_spent = total_spent;
            update_result(&res, navigate_page(data->page_ctx, idx,
                                              (const void *)res_args,
                                              sizeof(*res_args)));
            *cont = INPUTC_EXIT;
          }
        }
      } else if (data->cur == 3) {
        update_result(&res, popback_page(data->page_ctx));
        *cont = INPUTC_EXIT;
      } else {
        *cont = INPUTC_CONTINUE;
      }
    } else if (c == '\x7f') {
      if (data->cur == 0) {
        if (filename_len > 0) {
          filename[filename_len - 1] = '\0';
          update_result(&res,
                        draw_view(data->page_ctx->ctx, data->cur, filename,
                                  number_of_games, NOTHING_MULTI));
          filename_len--;
        }
        *cont = INPUTC_CONTINUE;
      } else if (data->cur == 1) {
        if (number_of_games_len > 0) {
          number_of_games[number_of_games_len - 1] = '\0';
          update_result(&res,
                        draw_view(data->page_ctx->ctx, data->cur, filename,
                                  number_of_games, NOTHING_MULTI));
          number_of_games_len--;
        }
        *cont = INPUTC_CONTINUE;
      } else {
        *cont = INPUTC_CONTINUE;
      }
    } else {
      if (data->cur == 0) {
        filename[filename_len] = c;
        filename[filename_len + 1] = '\0';
        filename_len++;
        update_result(&res, draw_view(data->page_ctx->ctx, data->cur, filename,
                                      number_of_games, NOTHING_MULTI));
        *cont = INPUTC_CONTINUE;
      } else if (data->cur == 1) {
        number_of_games[number_of_games_len] = c;
        number_of_games[number_of_games_len + 1] = '\0';
        number_of_games_len++;
        update_result(&res, draw_view(data->page_ctx->ctx, data->cur, filename,
                                      number_of_games, NOTHING_MULTI));
        *cont = INPUTC_CONTINUE;
      } else {
        *cont = INPUTC_CONTINUE;
      }
    }
  }
  return res;
}

enum ConsoleResult on_view_multi_game(struct PageCtx *page_ctx, void *args) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  char *filename = malloc(sizeof(char) * 4096);
  char *number_of_games = malloc(sizeof(char) * 4096);
  filename[0] = '\0';
  number_of_games[0] = '\0';
  struct HandleKeyDataMulti *data = malloc(sizeof(struct HandleKeyDataMulti));
  data->page_ctx = page_ctx;
  data->filename = filename;
  data->number_of_games = number_of_games;
  data->cur = 0;
  update_result(&res, draw_view(page_ctx->ctx, data->cur, data->filename,
                                data->number_of_games, NOTHING_MULTI));
  pthread_join(handle_input_count(page_ctx->ctx, INPUT_DEFAULT, 1,
                                  handle_key_multi_game, (void *)data),
               NULL);
  free(filename);
  free(number_of_games);
  return res;
}
