// SPDX-License-Identifier: MIT

#ifndef INPUT_H
#define INPUT_H

#include "console.h"
#include <pthread.h>

typedef enum ConsoleResult (*handle_key)(char *const, void *);
typedef enum ConsoleResult (*handle_another_key)(char *const, void *);

enum InputOptions {
  DEFAULT = 0b0,
  BACKSPACE_ALLOWED = 0b10,
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
extern pthread_t handleInputArrow(struct ConsoleCtx *ctx, handle_key func,
                                  handle_another_key func_another,
                                  void *inner_args);
extern pthread_t handleInputLine(struct ConsoleCtx *ctx, handle_key func,
                                 void *inner_args);
extern pthread_t handleInputCount(struct ConsoleCtx *ctx,
                                  enum InputOptions options, size_t count,
                                  handle_key func, void *inner_args);

// PRIVATE fields
void handleInputArrowInner(void *args);
void handleInputLineInner(void *args);
void handleInputCountInner(void *args);

#endif