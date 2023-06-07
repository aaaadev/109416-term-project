// SPDX-License-Identifier: MIT

#include "menu.h"
#include "../debug.h"
#include "console.h"
#include "input.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// PRIVATE
enum ConsoleResult key_event_another(const char *str, void *args,
                                     enum InputControll *cont) {
  char c = str[0];
  struct MenuCtx *ctx = args;
  switch (c) {
  case '\n':
    // TODO
    ctx->items[ctx->current_select].is_selected = false;
    ctx->on_ok(ctx);
    *cont = INPUTC_EXIT;
    break;
  default:
    break;
  }

  return CRESULT_SUCCESS;
}

// PRIVATE
enum ConsoleResult key_event(const char *str, void *args,
                             enum InputControll *cont) {
  char c = str[0];
  struct MenuCtx *ctx = args;
  switch (c) {
  case 'A':
    // UP
    if (ctx->current_select != 0) {
      ctx->items[ctx->current_select].is_selected = false;
      ctx->current_select--;
      ctx->items[ctx->current_select].is_selected = true;
      update_menu(ctx);
    }
    *cont = INPUTC_CONTINUE;
    break;
  case 'B':
    // DOWN
    if (ctx->current_select != ctx->item_count - 1) {
      ctx->items[ctx->current_select].is_selected = false;
      ctx->current_select++;
      ctx->items[ctx->current_select].is_selected = true;
      update_menu(ctx);
    }
    *cont = INPUTC_CONTINUE;
    break;
  case 'C':
    *cont = INPUTC_CONTINUE;
    // RIGHT
    break;
  case 'D':
    *cont = INPUTC_CONTINUE;
    // LEFT
    break;
  default:
    break;
  }
  return CRESULT_SUCCESS;
}

struct MenuCtx *menu_init(struct ConsoleCtx *ctx, struct PageCtx *page_ctx,
                          const char *restrict name, const char *restrict body,
                          on_ok_function on_ok) {
  struct MenuCtx *menu = malloc(sizeof(struct ConsoleCtx));
  menu->name = name;
  menu->body = body;
  menu->ctx = ctx;
  menu->page_ctx = page_ctx;
  menu->item_count = 0;
  menu->current_select = 0;
  menu->items = NULL;
  menu->on_ok = on_ok;
  return menu;
}

size_t add_item(struct MenuCtx *ctx, const struct MenuItem item) {
  ctx->items =
      realloc(ctx->items, sizeof(struct MenuItem) * (ctx->item_count + 10));
  ctx->items[ctx->item_count] = (struct MenuItem)item;
  return (++ctx->item_count);
}

enum ConsoleResult on_view_menu(struct PageCtx *page_ctx, void *args) {
  struct MenuCtx *ctx = (struct MenuCtx *)args;
  enum ConsoleResult result = CRESULT_SUCCESS;
#ifdef DEBUG
  if (ctx->item_count == 0)
    DPRINTF("warning: ctx->item_count is zero, can cause undefined behavior\n");
  fflush(stdout);
#endif
  ctx->items[ctx->current_select].is_selected = true;
  char *buffer =
      malloc(sizeof(char) * (strlen(ctx->name) + strlen(ctx->body) + 15));
  sprintf(buffer, "< %s >\n%s", ctx->name, ctx->body);
  size_t len = strlen(buffer);
  for (int i = 0; i < ctx->item_count; i++) {
    buffer[len] = '\n';
    buffer[len + 1] = '\0';
    buffer =
        realloc(buffer, sizeof(char) * (len + strlen(ctx->items[i].text) + 25));
    char *text = malloc(sizeof(char) * (strlen(ctx->items[i].text) + 10));
    text[0] = '\0';
    if (ctx->items[i].is_selected) {
      strcat(text, "> ");
    }
    strcat(text, ctx->items[i].text);
    size_t src_len = strlen(text);
    strcat(buffer, text);
    len += src_len + 1;
    free(text);
  }
  // update_result(&result, clear_all(ctx->ctx));
  update_result(&result, simple_textvh(ctx->ctx, buffer, HORIZONTAL_CENTER,
                                       VERTICAL_CENTER));
  update_result(&result, set_cursor(ctx->ctx, (struct Cursor){0, 0}));

  // Key handle
  void **ret = malloc(sizeof(enum ConsoleResult));
  *ret = (void *)CRESULT_SUCCESS;
  pthread_t handler =
      handle_input_arrow(ctx->ctx, key_event, key_event_another, ctx);
  pthread_join(handler, ret);
  update_result(&result, (enum ConsoleResult)(*ret));
  return result;
}

enum ConsoleResult update_menu(struct MenuCtx *ctx) {
  enum ConsoleResult result = CRESULT_SUCCESS;
#ifdef DEBUG
  if (ctx->item_count == 0)
    DPRINTF("warning: ctx->item_count is zero, can cause undefined behavior\n");
  fflush(stdout);
#endif
  char *buffer =
      malloc(sizeof(char) * (strlen(ctx->name) + strlen(ctx->body) + 15));
  sprintf(buffer, "< %s >\n%s", ctx->name, ctx->body);
  size_t len = strlen(buffer);
  for (int i = 0; i < ctx->item_count; i++) {
    buffer[len] = '\n';
    buffer[len + 1] = '\0';
    buffer =
        realloc(buffer, sizeof(char) * (len + strlen(ctx->items[i].text) + 25));
    char *text = malloc(sizeof(char) * (strlen(ctx->items[i].text) + 10));
    text[0] = '\0';
    if (ctx->items[i].is_selected) {
      strcat(text, "> ");
    }
    strcat(text, ctx->items[i].text);
    size_t src_len = strlen(text);
    strcat(buffer, text);
    len += src_len + 1;
    free(text);
  }
  // update_result(&result, clear_all(ctx->ctx));
  update_result(&result, simple_textvh(ctx->ctx, buffer, HORIZONTAL_CENTER,
                                       VERTICAL_CENTER));
  set_cursor(ctx->ctx, (struct Cursor){0, 0});
  return result;
}