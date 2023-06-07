// SPDX-License-Identifier: MIT

#ifndef MENU_H
#define MENU_H

#include "console.h"
#include "page.h"
#include <stdbool.h>

struct MenuCtx;
typedef enum ConsoleResult (*on_ok_function)(const struct MenuCtx *);

struct MenuItem {
  char *text;
  bool is_selected;
};

struct MenuCtx {
  const char *name;
  const char *body;
  struct ConsoleCtx *ctx;
  struct PageCtx *page_ctx;
  size_t item_count;
  struct MenuItem *items;
  size_t current_select;
  on_ok_function on_ok;
};

// PUBLIC fields

extern struct MenuCtx *menu_init(struct ConsoleCtx *ctx,
                                 struct PageCtx *page_ctx,
                                 const char *restrict name,
                                 const char *restrict body,
                                 on_ok_function on_ok);
extern size_t add_item(struct MenuCtx *ctx, const struct MenuItem item);
extern enum ConsoleResult on_view_menu(struct PageCtx *page_ctx, void *args);
extern enum ConsoleResult update_menu(struct MenuCtx *ctx);

// PRIVATE fields

#endif