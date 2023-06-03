// SPDX-License-Identifier: MIT

#include "../console/console.h"
#include "../console/dialog.h"
#include "../console/menu.h"
#include "../console/page.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const unsigned int DELAY_SECS = 5;

enum ConsoleResult on_ok_test(const struct MenuCtx *menu) {
  struct OkDialogConfig cfg = {
      "on_ok_text",
      "this is a test",
  };
  enum ConsoleResult res = CRESULT_SUCCESS;
  size_t idx = add_page(menu->page_ctx, on_view_ok_dialog, false);
  update_result(&res, navigate_page(menu->page_ctx, idx, (void *)&cfg,
                                    sizeof(struct OkDialogConfig)));
  return res;
}

int main(void) {
  struct ConsoleCtx *console = console_init();
  struct PageCtx *page = page_init(console);
  simple_textvh(console, "Loading", HORIZONTAL_CENTER, VERTICAL_CENTER);
  struct MenuCtx *menu = menu_init(console, page, "Test menu", on_ok_test);
  struct MenuItem first_item = (struct MenuItem){
      "this is first menu item",
      false,
  };
  struct MenuItem second_item = (struct MenuItem){
      "this is second menu item",
      false,
  };
  struct MenuItem third_item = (struct MenuItem){
      "this is third menu item",
      false,
  };
  add_item(menu, (const struct MenuItem)first_item);
  add_item(menu, (const struct MenuItem)second_item);
  add_item(menu, (const struct MenuItem)third_item);
  size_t idx = add_page(page, on_view_menu, false);
  navigate_page(page, idx, menu, sizeof(*menu));
  sleep(DELAY_SECS);
  console_shutdown(console);
  page_shutdown(page);
  return 0;
}