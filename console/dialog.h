// SPDX-License-Identifier: MIT

#ifndef DIALOG_H
#define DIALOG_H

#include "console.h"
#include "page.h"
#include "input.h"
#include <stdlib.h>

struct OkDialogConfig {
  const char *title;
  const char *body;
};

// PUBLIC fields

extern enum ConsoleResult on_view_ok_dialog(struct PageCtx *page_ctx,
                                            void *args);

// PRIVATE fields

enum ConsoleResult key_event_ok_dialog(const char * str, void *args, enum InputControll *cont);

#endif