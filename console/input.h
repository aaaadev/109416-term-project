// SPDX-License-Identifier: MIT

#ifndef INPUT_H
#define INPUT_H

#include "console.h"
#include <pthread.h>

typedef enum ConsoleResult (*handle_key)(const char *, void *, enum InputControll *);
typedef enum ConsoleResult (*handle_another_key)(const char *, void *, enum InputControll *);

enum InputOptions {
  INPUT_DEFAULT = 0b0,
  INPUT_BACKSPACE_ALLOWED = 0b10,
};

enum InputControll {
    INPUTC_CONTINUE = 0x00,
    INPUTC_EXIT = 0x01,
};

struct InputCtx {
  struct ConsoleCtx *ctx;
  handle_key func;
  handle_another_key func_another;
  void *args;
  // optional
  size_t count;
  // optional
  enum InputOptions options;
};

// PUBLIC fields
extern pthread_t handle_input_arrow(struct ConsoleCtx *ctx, handle_key func,
                                    handle_another_key func_another,
                                    void *inner_args);
extern pthread_t handle_input_line(struct ConsoleCtx *ctx, handle_key func,
                                   void *inner_args);
extern pthread_t handle_input_count(struct ConsoleCtx *ctx,
                                    enum InputOptions options, size_t count,
                                    handle_key func, void *inner_args);

// PRIVATE fields
void handle_input_arrow_inner(void *args);
void handle_input_line_inner(void *args);
void handle_input_count_inner(void *args);

#endif