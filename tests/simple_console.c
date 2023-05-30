// SPDX-License-Identifier: MIT

#include "../console/console.h"
#include "../console/menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const unsigned int DELAY_SECS = 5;

void on_ok_test(const struct Menu * menu) {
    #ifdef DEBUG
    printf("on_ok_test() called\n");
    fflush(stdout);
    #endif
    return;
}

int main(void) {
  struct ConsoleCtx *console = console_init();
  simple_textvh(console, "Loading", HORIZONTAL_CENTER, VERTICAL_CENTER);
  struct MenuCtx *menu = menu_init(console, "Test menu", on_ok_test);
  struct MenuItem first_item = (struct MenuItem) {
    "this is first menu item",
    false,
  };
  struct MenuItem second_item = (struct MenuItem) {
    "this is second menu item",
    false,
  };
  add_item(menu, (const struct MenuItem) first_item);
  add_item(menu, (const struct MenuItem) second_item);
  show_menu(menu);
  sleep(DELAY_SECS);
  console_shutdown(console);
  return 0;
}