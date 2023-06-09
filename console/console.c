// SPDX-License-Identifier: MIT

#include "console.h"
#include "../debug.h"
#include "../util/parse_string.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

struct ConsoleCtx *console_init(void) {
  struct ConsoleCtx *ctx = malloc(sizeof(struct ConsoleCtx));
  printf("\e[?25l");
  pthread_mutex_init(&ctx->window_mutex, NULL);
  enable_raw_mode(ctx);
  pthread_mutex_lock(&ctx->window_mutex);
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ctx->window.window_size);
#ifdef DEBUG
  DPRINTF("row: %d col: %d\n", ctx->window.window_size.ws_row,
          ctx->window.window_size.ws_col);
  fflush(stdout);
#endif
  ctx->window.data =
      (char **)malloc(sizeof(char *) * (ctx->window.window_size.ws_row + 1));
  for (int i = 0; i < ctx->window.window_size.ws_row; i++) {
    ctx->window.data[i] =
        (char *)malloc(sizeof(char) * (ctx->window.window_size.ws_col + 1));
  }
  ctx->window.prev_data =
      (char **)malloc(sizeof(char *) * (ctx->window.window_size.ws_row + 1));
  for (int i = 0; i < ctx->window.window_size.ws_row; i++) {
    ctx->window.prev_data[i] =
        (char *)malloc(sizeof(char) * (ctx->window.window_size.ws_col + 1));
  }
  pthread_mutex_unlock(&ctx->window_mutex);
  basic_draw(ctx);
  clear_all(ctx);
  refresh_window(ctx);
  // MUST BE FREED LATER
  return ctx;
}

enum ConsoleResult sync_cursor(struct ConsoleCtx *ctx) {
  printf("\033[6n");
  fflush(stdout);

  char buf[32];
  int ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
  if (ret > 0) {
    buf[ret] = '\0';
    sscanf(buf, "\033[%d;%dR", &ctx->cursor.x, &ctx->cursor.y);
  }

  // Handle 1-indexed
  ctx->cursor.x--;
  ctx->cursor.y--;

  return CRESULT_SUCCESS;
}

enum ConsoleResult basic_draw(struct ConsoleCtx *ctx) {
  pthread_mutex_lock(&ctx->window_mutex);
  for (int i = 0; i < ctx->window.window_size.ws_col; i++) {
    ctx->window.data[0][i] = '#';
    ctx->window.prev_data[0][i] = ' ';
  }
  for (int i = 1; i < ctx->window.window_size.ws_row - 1; i++) {
    ctx->window.data[i][0] = '#';
    ctx->window.prev_data[i][0] = ' ';
    for (int j = 1; j < ctx->window.window_size.ws_col - 1; j++) {
      ctx->window.data[i][j] = ' ';
      ctx->window.prev_data[i][j] = ' ';
    }
    ctx->window.data[i][ctx->window.window_size.ws_col - 1] = '#';
    ctx->window.prev_data[i][ctx->window.window_size.ws_col - 1] = ' ';
  }
  for (int i = 0; i < ctx->window.window_size.ws_col; i++) {
    ctx->window.data[ctx->window.window_size.ws_row - 1][i] = '#';
    ctx->window.prev_data[ctx->window.window_size.ws_row - 1][i] = ' ';
  }
  pthread_mutex_unlock(&ctx->window_mutex);
  return CRESULT_SUCCESS;
}

enum ConsoleResult refresh_window(struct ConsoleCtx *ctx) {
  enum ConsoleResult result = CRESULT_SUCCESS;
  update_result(&result, set_cursor(ctx, (struct Cursor){0, 0}));
  // raw_clear_console();
  pthread_mutex_lock(&ctx->window_mutex);
  for (int i = 0; i < ctx->window.window_size.ws_row; i++) {
    for (int j = 0; j < ctx->window.window_size.ws_col; j++) {
      update_result(&result, set_cursor(ctx, (struct Cursor){i, j}));
      if (ctx->window.prev_data[i][j] != ctx->window.data[i][j])
        printf("%c", ctx->window.data[i][j]);
      ctx->window.prev_data[i][j] = ctx->window.data[i][j];
    }
  }
  pthread_mutex_unlock(&ctx->window_mutex);
  fflush(stdout);
  update_result(&result, set_cursor(ctx, (struct Cursor){0, 0}));
  return result;
}

enum ConsoleResult suspend_console(struct ConsoleCtx *ctx) {
  raw_clear_console();
  printf("\e[?25h");
  set_cursor(ctx, (struct Cursor){0, 0});
  disable_raw_mode(ctx);
  pthread_mutex_lock(&ctx->window_mutex);
  for (int i = 0; i < ctx->window.window_size.ws_row; i++) {
    for (int j = 0; j < ctx->window.window_size.ws_col; j++) {
      ctx->window.prev_data[i][j] = '\0';
    }
  }
  pthread_mutex_unlock(&ctx->window_mutex);
  return CRESULT_SUCCESS;
}

enum ConsoleResult resume_console(struct ConsoleCtx *ctx) {
  enable_raw_mode(ctx);
  raw_clear_console();
  set_cursor(ctx, (struct Cursor){0, 0});
  printf("\e[?25l");
  refresh_window(ctx);
  return CRESULT_SUCCESS;
}

enum ConsoleResult console_shutdown(struct ConsoleCtx *restrict ctx) {
  printf("\e[?25h");
  disable_raw_mode(ctx);
  pthread_mutex_destroy(&ctx->window_mutex);
  free(ctx);
  return CRESULT_SUCCESS;
}

enum ConsoleResult set_cursor(struct ConsoleCtx *ctx, struct Cursor cur) {
  ctx->cursor = cur;
  raw_set_cursor(cur.x, cur.y);
  return CRESULT_SUCCESS;
}

enum ConsoleResult basic_text(struct ConsoleCtx *ctx, struct Cursor cur,
                              char *restrict text) {
  set_cursor(ctx, cur);
  int end_y = cur.y + strlen(text);
  pthread_mutex_lock(&ctx->window_mutex);
  if (end_y > ctx->window.window_size.ws_col - 1) {
    pthread_mutex_unlock(&ctx->window_mutex);
    return CRESULT_TOOLONGTEXT;
  } else {
    for (int i = cur.y; i < end_y; i++) {
      ctx->window.data[cur.x][i] = text[i - cur.y];
      // printf("%c", ctx->window.data[cur.x][i]);
    }
    pthread_mutex_unlock(&ctx->window_mutex);
    refresh_window(ctx);
    sync_cursor(ctx);
    return CRESULT_SUCCESS;
  }
}

enum ConsoleResult simple_textvh(struct ConsoleCtx *ctx, char *text,
                                 enum TextAlignHorizontal align_h,
                                 enum TextAlignVertical align_v) {
  enum ConsoleResult result = CRESULT_SUCCESS;
  int len = strlen(text);
  pthread_mutex_lock(&ctx->window_mutex);
  int col = ctx->window.window_size.ws_col;
  for (int i = 1; i < ctx->window.window_size.ws_row - 1; i++) {
    for (int j = 1; j < col - 1; j++) {
      ctx->window.prev_data[i][j] = ctx->window.data[i][j];
      ctx->window.data[i][j] = ' ';
    }
  }
  pthread_mutex_unlock(&ctx->window_mutex);
  struct ParseResult text_parse = parse_new_line(text, col - 2);
  const char **list = text_parse.list;
  size_t lines = text_parse.lines;
  int last_idx = 0;
  switch (align_v) {
  case VERTICAL_TOP:
    last_idx = 1 + lines - 1;
    for (int i = 1; i <= last_idx; i++) {
      update_result(&result, simple_texth(ctx, list[i - 1], i, align_h));
    }
    break;
  case VERTICAL_CENTER:
    last_idx = (ctx->window.window_size.ws_row - lines) / 2 + lines - 1;
    for (int i = (ctx->window.window_size.ws_row - lines) / 2; i <= last_idx;
         i++) {
      update_result(
          &result,
          simple_texth(ctx,
                       list[i - ((ctx->window.window_size.ws_row - lines) / 2)],
                       i, align_h));
    }
    break;
  default:
#ifdef DEBUG
    DPRINTF("undefined case reached\n");
    fflush(stdout);
#endif
    break;
  }
  return result;
}

enum ConsoleResult simple_texth(struct ConsoleCtx *ctx, char *restrict text,
                                int row, enum TextAlignHorizontal align_h) {
  enum ConsoleResult result = CRESULT_SUCCESS;
  int len = strlen(text);
  switch (align_h) {
  case HORIZONTAL_LEFT:
    update_result(&result, basic_text(ctx, (struct Cursor){row, 1}, text));
    break;
  case HORIZONTAL_CENTER:
    pthread_mutex_lock(&ctx->window_mutex);
    if (len > ctx->window.window_size.ws_col - 2) {
      pthread_mutex_unlock(&ctx->window_mutex);
      update_result(&result, CRESULT_TOOLONGTEXT);
    } else {
      pthread_mutex_unlock(&ctx->window_mutex);
      update_result(
          &result,
          basic_text(
              ctx,
              (struct Cursor){row, (ctx->window.window_size.ws_col - len) / 2},
              text));
    }
    break;
  case HORIZONTAL_RIGHT:
    pthread_mutex_lock(&ctx->window_mutex);
    if (len > ctx->window.window_size.ws_col - 2) {
      pthread_mutex_unlock(&ctx->window_mutex);
      update_result(&result, CRESULT_TOOLONGTEXT);
    } else {
      pthread_mutex_unlock(&ctx->window_mutex);
      update_result(
          &result,
          basic_text(
              ctx,
              (struct Cursor){row, ctx->window.window_size.ws_col - 1 - len},
              text));
    }
    break;
  default:
    break;
  }
  return CRESULT_SUCCESS;
}

enum ConsoleResult clear_line(struct ConsoleCtx *ctx, int row) {
  pthread_mutex_lock(&ctx->window_mutex);
  int col = ctx->window.window_size.ws_col;
  pthread_mutex_unlock(&ctx->window_mutex);
  set_cursor(ctx, (struct Cursor){row, 1});
  pthread_mutex_lock(&ctx->window_mutex);
  for (int i = 1; i < col - 1; i++) {
    ctx->window.data[row][i] = ' ';
    printf(" ");
  }
  pthread_mutex_unlock(&ctx->window_mutex);
  sync_cursor(ctx);
  fflush(stdout);
  return CRESULT_SUCCESS;
}

enum ConsoleResult clear_all(struct ConsoleCtx *ctx) {
  enum ConsoleResult result = CRESULT_SUCCESS;
  pthread_mutex_lock(&ctx->window_mutex);
  int row = ctx->window.window_size.ws_row - 1;
  pthread_mutex_unlock(&ctx->window_mutex);
  for (int i = 1; i < row; i++) {
    update_result(&result, clear_line(ctx, i));
  }
  return result;
}

// PRIVATE
void raw_clear_console() {
  printf("\033[2J\033[H");
  return;
}

// PRIVATE
void raw_set_cursor(int x, int y) { printf("\033[%d;%dH", x + 1, y + 1); }

// PRIVATE
void enable_raw_mode(struct ConsoleCtx *ctx) {
  struct termios raw;
  tcgetattr(STDIN_FILENO, &ctx->original_termios);
  raw = ctx->original_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VERASE] = '\b';
  raw.c_cc[VEOF] = '\n';
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  ctx->current = raw;
  return;
}

// PRIVATE
void disable_raw_mode(struct ConsoleCtx *ctx) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->original_termios);
  return;
}