// SPDX-License-Identifier: MIT

#include "console/console.h"
#include "console/menu.h"
#include "console/page.h"
#include "console/dialog.h"
#include "pages/main_menu.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

const unsigned int DELAY_SECS = 1;

int main(void) {
  struct ConsoleCtx *console = console_init();
  struct PageCtx *page = page_init(console);
  if (!(console->window.window_size.ws_col >= MIN_COLS && console->window.window_size.ws_row >= MIN_ROWS)) {
    char msg[4096];
    sprintf(msg, "Sorry. This program requires %d x %d terminal size to run.", MIN_COLS, MIN_ROWS);
    simple_textvh(console, msg, HORIZONTAL_CENTER, VERTICAL_CENTER);
    console_shutdown(console);
    page_shutdown(page);
    return 0;
  }
  simple_textvh(console, "Loading", HORIZONTAL_CENTER, VERTICAL_CENTER);
  sleep(DELAY_SECS);
  size_t idx = add_page(page, on_view_main, true);
  navigate_page(page, idx, NULL, 0);
  while (1) {
  }
  console_shutdown(console);
  page_shutdown(page);
  return 0;
}