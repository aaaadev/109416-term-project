// SPDX-License-Identifier: MIT

#include "input.h"
#include "../debug.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_t handle_input_arrow(struct ConsoleCtx *ctx, handle_key func,
                             handle_another_key func_another,
                             void *inner_args) {
  pthread_t handler;
  struct InputCtx *args = malloc(sizeof(struct InputCtx));
  args->ctx = ctx;
  args->func = func;
  args->func_another = func_another;
  args->count = 1;
  args->options = INPUT_DEFAULT;
  args->args = inner_args;
  pthread_create(&handler, NULL, handle_input_arrow_inner, (void *)args);
  return handler;
}

pthread_t handle_input_line(struct ConsoleCtx *ctx, handle_key func,
                            void *inner_args) {
  pthread_t handler;
  struct InputCtx *args = malloc(sizeof(struct InputCtx));
  args->ctx = ctx;
  args->func = func;
  args->options = INPUT_DEFAULT;
  args->args = inner_args;
  pthread_create(&handler, NULL, handle_input_line_inner, (void *)args);
  return handler;
}

pthread_t handle_input_count(struct ConsoleCtx *ctx, enum InputOptions options,
                             size_t count, handle_key func, void *inner_args) {
  pthread_t handler;
  struct InputCtx *args = malloc(sizeof(struct InputCtx));
  args->ctx = ctx;
  args->func = func;
  args->options = INPUT_DEFAULT;
  args->args = inner_args;
  args->count = count;
  pthread_create(&handler, NULL, handle_input_count_inner, (void *)args);
  return handler;
}

// PRIVATE

void handle_input_arrow_inner(void *args) {
  struct InputCtx ctx = *((struct InputCtx *)(args));
  char c = '\0';
  while (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == '\033') {
      read(STDIN_FILENO, &c, 1);
      if (c == '[') {
        read(STDIN_FILENO, &c, 1);
        char *str = malloc(sizeof(char) * 2);
        str[0] = c;
        str[1] = '\0';
        enum InputControll cont = INPUTC_CONTINUE;
        ctx.func(str, ctx.args, &cont);
        if (cont == INPUTC_EXIT)
          break;
      }
    } else {
      char *str = malloc(sizeof(char) * 2);
      str[0] = c;
      str[1] = '\0';
      enum InputControll cont = INPUTC_CONTINUE;
      ctx.func_another(str, ctx.args, &cont);
      if (cont == INPUTC_EXIT)
        break;
    }
  }
  enum ConsoleResult res = CRESULT_SUCCESS;
  pthread_exit((void *)res);
}

// PRIVATE

void handle_input_line_inner(void *args) {
  struct InputCtx ctx = *((struct InputCtx *)(args));
  char *input = NULL;
  size_t bufsize = 0;
  if (getline(input, &bufsize, stdin) != -1) {
    char *newline = strchr(input, '\n');
    if (newline != NULL) {
      *newline = '\0';
      enum InputControll cont = INPUTC_CONTINUE;
      ctx.func(input, ctx.args, &cont);
    } else {
#ifdef DEBUG
      printf("error: buffer size overflow occurred. expected size: %ld\n",
             ctx.count);
      fflush(stdout);
#endif
      enum ConsoleResult res = CRESULT_FAILURE;
      pthread_exit((void *)res);
    }
  }
  enum ConsoleResult res = CRESULT_SUCCESS;
  pthread_exit((void *)res);
}

// PRIVATE

void handle_input_count_inner(void *args) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  struct InputCtx ctx = *((struct InputCtx *)(args));
  char *str = malloc(sizeof(char) * (ctx.count + 1));
  char c = '\0';
  while (1) {
    size_t cnt = 0;
    while (cnt < ctx.count && (c = getchar()) != EOF) {
      str[cnt] = c;
      cnt++;
    }
    str[cnt] = '\0';
    enum InputControll cont = INPUTC_CONTINUE;
    update_result(&res, ctx.func(str, ctx.args, &cont));
    if (cont == INPUTC_EXIT)
      break;
  }
  pthread_exit((void *)res);
  return;
}