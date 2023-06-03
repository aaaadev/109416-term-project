// SPDX-License-Identifier: MIT

#include "dialog.h"
#include "../debug.h"
#include "console.h"
#include "input.h"
#include "page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum ConsoleResult on_view_ok_dialog(struct PageCtx *page_ctx, void *args) {
  struct OkDialogConfig *cfg = (struct OkDialogConfig *)args;
  struct ConsoleCtx *ctx = page_ctx->ctx;
  pthread_mutex_lock(&ctx->window_mutex);
  int col = ctx->window.window_size.ws_col;
  pthread_mutex_unlock(&ctx->window_mutex);
  char *text =
      malloc(sizeof(char) * (strlen(cfg->title) + 6 + strlen(cfg->body) + 10));
  sprintf(text, "< %s >", (const char *)cfg->title);
  size_t len = strlen(text);
  text[len] = '\n';
  text[len + 1] = '\0';
  strcat(text, cfg->body);
  len = strlen(text);
  text[len] = '\n';
  text[len + 1] = '\0';
  strcat(text, "> ok\n");
  clear_all(ctx);
  simple_textvh(ctx, text, HORIZONTAL_CENTER, VERTICAL_CENTER);
  free(text);
  handle_input_count(ctx, INPUT_DEFAULT, 1, key_event_ok_dialog, page_ctx);
  return CRESULT_SUCCESS;
}

enum ConsoleResult key_event_ok_dialog(const char *str, void *args,
                                       enum InputControll *cont) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  struct PageCtx *ctx = args;
  char c = str[0];
  switch (c) {
  case '\n':
    update_result(&res, popback_page(ctx));
    *cont = INPUTC_EXIT;
    break;
  default:
#ifdef DEBUG
    DPRINTF("unknown key %x detected during key_event_ok_dialog()\n", c);
#endif
    *cont = INPUTC_CONTINUE;
    break;
  }
  return res;
}