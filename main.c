// SPDX-License-Identifier: MIT

#include "console/console.h"
#include "console/menu.h"
#include "console/page.h"
#include "console/dialog.h"
#include "pages/main_menu.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  struct ConsoleCtx *console = console_init();
  struct PageCtx *page = page_init(console);
  simple_textvh(console, "Loading", HORIZONTAL_CENTER, VERTICAL_CENTER);
  size_t idx = add_page(page, on_view_main, true);
  navigate_page(page, idx, NULL, 0);
  while (1) {
  }
  console_shutdown(console);
  return 0;
}