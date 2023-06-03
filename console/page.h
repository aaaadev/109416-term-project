// SPDX-License-Identifier: MIT

#ifndef PAGE_H
#define PAGE_H

#include "console.h"
#include <pthread.h>
#include <stdlib.h>

typedef enum ConsoleResult (*page_func)(struct PageCtx *, void *);

struct PageItem {
  size_t page_num;
  page_func page_draw;
};

struct PageHistory {
  size_t page_num;
  size_t args_size;
};

struct PageCtx {
  struct ConsoleCtx *ctx;
  pthread_mutex_t page_mutex;
  size_t page_count;
  size_t cur_page;
  struct PageItem *pages;
  struct PageHistory *history;
  size_t history_size;
  size_t history_args_size;
  void *history_args;
};

// PUBLIC fields

extern struct PageCtx *page_init(struct ConsoleCtx *ctx);
extern enum ConsoleResult page_shutdown(struct PageCtx *ctx);
extern size_t add_page(struct PageCtx *ctx, page_func page_view);
extern enum ConsoleResult navigate_page(struct PageCtx *ctx, size_t page_num,
                                        const void *args, size_t args_size);
extern enum ConsoleResult popback_page(struct PageCtx *ctx);

#endif