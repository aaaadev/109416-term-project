// SPDX-License-Identifier: MIT

#include "single_game_file.h"
#include "../console/console.h"
#include "../console/input.h"
#include "../console/page.h"
#include "../util/check_duplicate.h"
#include "../util/generate_numbers.h"
#include "../util/split_whitespace.h"
#include "single_game_result.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// PRIVATE
static enum ConsoleResult draw_view(struct ConsoleCtx *ctx, size_t cur,
                                    const char *str, enum FileError err) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  char *msg =
      malloc(sizeof(char) * (strlen(MSG_FILE) + 4096 + strlen(str) + 4096));
  msg[0] = '\0';
  switch (err) {
  case NOTHING:
    strcat(msg, MSG_FILE);
    break;
  case INVALID_FILE_NAME:
    strcat(msg, MSG_INVALID_FILE_FORMAT_FILE);
    break;
  case INVALID_FILE_FORMAT:
    strcat(msg, MSG_INVALID_FILE_FORMAT_FILE);
    break;
  default:
    break;
  }
  if (cur == 0) {
    strcat(msg, "> input: ");
    strcat(msg, str);
    strcat(msg, "\nok");
    strcat(msg, "\nback");
  } else if (cur == 1) {
    strcat(msg, "input: ");
    strcat(msg, str);
    strcat(msg, "\n> ok");
    strcat(msg, "\nback");
  } else if (cur == 2) {
    strcat(msg, "input: ");
    strcat(msg, str);
    strcat(msg, "\nok");
    strcat(msg, "\n> back");
  }
  update_result(&res,
                simple_textvh(ctx, msg, HORIZONTAL_CENTER, VERTICAL_CENTER));
  free(msg);
  return res;
}

// PRIVATE
enum ConsoleResult handle_key_single_game_file(const char *text, void *args,
                                               enum InputControll *cont) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  char c = text[0];
  struct HandleKeyDataFile *data = (struct HandleKeyDataFile *)args;
  char *str = data->str;
  size_t len = strlen(str);
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
        if (data->cur != 2) {
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
      update_result(&res,
                    draw_view(data->page_ctx->ctx, data->cur, str, NOTHING));
      *cont = INPUTC_CONTINUE;
    }

  } else {
    if (c == '\n') {
      if (data->cur == 1) {
        FILE *file = fopen(str, "r");
        if (!file) {
          update_result(&res, draw_view(data->page_ctx->ctx, data->cur, str,
                                        INVALID_FILE_NAME));
          *cont = INPUTC_CONTINUE;
        } else {
          char *line = malloc(sizeof(char) * 4096);
          if (fgets(line, 4096, file) == NULL) {
            update_result(&res, draw_view(data->page_ctx->ctx, data->cur, str,
                                          INVALID_FILE_FORMAT));
            *cont = INPUTC_CONTINUE;
          } else {
            struct SplitResult split = split_whitespace((const char *)line);
            if (split.length != 6) {
              if (split.length == 1 && strlen(split.data[0]) == 1 &&
                  split.data[0][0] == 'A') {
                // automatic
                uint64_t *user_nums = generate_numbers();
                uint64_t *winning_nums = generate_numbers();
                struct ResultArgs *res_args = malloc(sizeof(struct ResultArgs));
                res_args->page_ctx = data->page_ctx;
                res_args->user_nums = user_nums;
                res_args->winning_nums = winning_nums;
                size_t idx =
                    add_page(data->page_ctx, on_view_single_game_result, true);
                update_result(&res, navigate_page(data->page_ctx, idx, res_args,
                                                  sizeof(*res_args)));
                *cont = INPUTC_EXIT;
              } else {
                update_result(&res, draw_view(data->page_ctx->ctx, data->cur,
                                              str, INVALID_FILE_FORMAT));
                *cont = INPUTC_CONTINUE;
              }
            } else {
              uint64_t *num = malloc(sizeof(uint64_t) * 6);
              if (sscanf(line, "%lld %lld %lld %lld %lld %lld", &num[0],
                         &num[1], &num[2], &num[3], &num[4], &num[5],
                         &num[6]) == 6) {
                bool range_check = true;
                bool duplicate_check = check_duplicate(num);
                for (int i = 0; i < 6; i++) {
                  if (!(num[i] >= 1 && num[i] <= 45)) {
                    range_check = false;
                  }
                }
                if (!range_check || duplicate_check) {
                  update_result(&res, draw_view(data->page_ctx->ctx, data->cur,
                                                str, INVALID_FILE_FORMAT));
                  *cont = INPUTC_CONTINUE;
                } else {
                  uint64_t *winning_nums = generate_numbers();
                  struct ResultArgs *res_args =
                      malloc(sizeof(struct ResultArgs));
                  res_args->page_ctx = data->page_ctx;
                  res_args->user_nums = num;
                  res_args->winning_nums = winning_nums;
                  size_t idx = add_page(data->page_ctx,
                                        on_view_single_game_result, true);
                  update_result(&res,
                                navigate_page(data->page_ctx, idx, res_args,
                                              sizeof(*res_args)));
                  *cont = INPUTC_EXIT;
                }
              } else {
                update_result(&res, draw_view(data->page_ctx->ctx, data->cur,
                                              str, INVALID_FILE_FORMAT));
                *cont = INPUTC_CONTINUE;
              }
            }
          }
        }
        fclose(file);
      } else if (data->cur == 2) {
        update_result(&res, popback_page(data->page_ctx));
        *cont = INPUTC_EXIT;
      } else {
        *cont = INPUTC_CONTINUE;
      }
    } else if (c == '\x7f') {
      if (data->cur == 0) {
        if (len > 0) {
          str[len - 1] = '\0';
          update_result(
              &res, draw_view(data->page_ctx->ctx, data->cur, str, NOTHING));
        }
        *cont = INPUTC_CONTINUE;
      } else {
        *cont = INPUTC_CONTINUE;
      }
    } else {
      if (data->cur == 0) {
        str[len] = c;
        str[len + 1] = '\0';
        len++;
        update_result(&res,
                      draw_view(data->page_ctx->ctx, data->cur, str, NOTHING));
        *cont = INPUTC_CONTINUE;
      } else {
        *cont = INPUTC_CONTINUE;
      }
    }
  }
  return res;
}

enum ConsoleResult on_view_single_game_file(struct PageCtx *page_ctx,
                                            void *args) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  char *str = malloc(sizeof(char) * 4096);
  str[0] = '\0';
  struct HandleKeyDataFile *data = malloc(sizeof(struct HandleKeyDataFile));
  data->page_ctx = page_ctx;
  data->str = str;
  data->cur = 0;
  update_result(&res, draw_view(page_ctx->ctx, data->cur, data->str, NOTHING));
  pthread_join(handle_input_count(page_ctx->ctx, INPUT_DEFAULT, 1,
                                  handle_key_single_game_file, (void *)data),
               NULL);
  free(str);
  return res;
}
