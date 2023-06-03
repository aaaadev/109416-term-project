// SPDX-License-Identifier: MIT

#include "page.h"
#include "../debug.h"
#include "console.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PageCtx *page_init(struct ConsoleCtx *ctx) {
  struct PageCtx *page_ctx = malloc(sizeof(struct PageCtx));
  page_ctx->ctx = ctx;
  pthread_mutex_init(&page_ctx->page_mutex, NULL);
  page_ctx->page_count = 0;
  page_ctx->cur_page = 0;
  page_ctx->pages = NULL;
  page_ctx->history = NULL;
  page_ctx->history_args_size = 0;
  page_ctx->history_args = NULL;
  page_ctx->history_size = 0;
  return page_ctx;
}

size_t add_page(struct PageCtx *ctx, page_func page_view) {
  pthread_mutex_lock(&ctx->page_mutex);
  ctx->pages =
      realloc(ctx->pages, sizeof(struct PageItem) * (ctx->page_count + 2));
  ctx->pages[ctx->page_count] = (struct PageItem){ctx->page_count, page_view};
  ctx->page_count++;
  pthread_mutex_unlock(&ctx->page_mutex);
  return ctx->page_count - 1;
}

enum ConsoleResult navigate_page(struct PageCtx *ctx, size_t page_num,
                                 const void *args, size_t args_size) {
  pthread_mutex_lock(&ctx->page_mutex);
  if (page_num >= ctx->page_count) {
#ifdef DEBUG
    DPRINTF("warning: page_num(%Ld) >= ctx->page_count(%Ld)\n", page_num,
            ctx->page_count);
    fflush(stdout);
#endif
    pthread_mutex_unlock(&ctx->page_mutex);
    return CRESULT_PAGENOTAVAILABLE;
  }

  ctx->history = realloc(ctx->history,
                         sizeof(struct PageHistory) * (ctx->history_size + 2));
  ctx->history[ctx->history_size] = (struct PageHistory){page_num, args_size};
  ctx->history_size++;
  ctx->history_args =
      realloc(ctx->history_args, ctx->history_args_size + args_size);
  memcpy(ctx->history_args + ctx->history_args_size, args, args_size);
  ctx->history_args_size += args_size;
  enum ConsoleResult result = CRESULT_SUCCESS;
  pthread_mutex_unlock(&ctx->page_mutex);
  update_result(&result, ctx->pages[page_num].page_draw(ctx, args));
  return result;
}

enum ConsoleResult popback_page(struct PageCtx *ctx) {
  pthread_mutex_lock(&ctx->page_mutex);
  if (ctx->page_count == 0) {
#ifdef DEBUG
    DPRINTF("warning: ctx->page_count == 0\n");
    fflush(stdout);
#endif
    pthread_mutex_unlock(&ctx->page_mutex);
    return CRESULT_PAGENOTAVAILABLE;
  }
  ctx->history_size--;
  void *args = malloc(ctx->history[ctx->history_size].args_size);
  ctx->history_args_size -= ctx->history[ctx->history_size].args_size;
  memcpy(args, ctx->history_args + ctx->history_args_size,
         ctx->history[ctx->history_size].args_size);
  ctx->history_args = realloc(ctx->history_args, ctx->history_args_size);
  enum ConsoleResult result = CRESULT_SUCCESS;
  pthread_mutex_unlock(&ctx->page_mutex);
  update_result(&result,
                ctx->pages[ctx->history[ctx->history_size].page_num].page_draw(
                    ctx, args));
  pthread_mutex_lock(&ctx->page_mutex);
  ctx->history = realloc(ctx->history, ctx->history_size + 2);
  pthread_mutex_unlock(&ctx->page_mutex);
  return result;
}