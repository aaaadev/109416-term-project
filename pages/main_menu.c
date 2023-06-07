// SPDX-License-Identifier: MIT

#include "main_menu.h"
#include "../console/console.h"
#include "../console/menu.h"
#include "../console/page.h"
#include "../debug.h"
#include "about_page.h"
#include "multi_game.h"
#include "single_game.h"
#include "single_game_file.h"
#include <stdio.h>
#include <stdlib.h>

// PRIVATE

enum ConsoleResult on_ok_main_menu(const struct MenuCtx *menu) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  struct PageCtx *page_ctx = menu->page_ctx;
  if (menu->current_select == 3) {
    size_t idx = add_page(page_ctx, on_view_about_page, true);
    update_result(&res, navigate_page(page_ctx, idx, NULL, 0));
  } else if (menu->current_select == 0) {
    size_t idx = add_page(page_ctx, on_view_single_game, false);
    update_result(&res, navigate_page(page_ctx, idx, NULL, 0));
  } else if (menu->current_select == 1) {
    size_t idx = add_page(page_ctx, on_view_single_game_file, false);
    update_result(&res, navigate_page(page_ctx, idx, NULL, 0));
  } else if (menu->current_select == 2) {
    size_t idx = add_page(page_ctx, on_view_multi_game, false);
    update_result(&res, navigate_page(page_ctx, idx, NULL, 0));
  } else {
#ifdef DEBUG
    DPRINTF("unknown item select handle\n");
    fflush(stdout);
#endif
  }
  return res;
}

enum ConsoleResult on_view_main(struct PageCtx *page_ctx, void *args) {
  // main_menu dummy page
  enum ConsoleResult res = CRESULT_SUCCESS;
  struct MenuCtx *menu = menu_init(page_ctx->ctx, page_ctx,
                                   "Lotto 6/45 simulator", "", on_ok_main_menu);
  struct MenuItem first_item = (struct MenuItem){
      "1. Single game mode (using keyboard input)",
      false,
  };
  struct MenuItem second_item = (struct MenuItem){
      "2. Single game mode (using file input)",
      false,
  };
  struct MenuItem third_item = (struct MenuItem){
      "3. Multi game simulation mode",
      false,
  };
  struct MenuItem forth_item = (struct MenuItem){
      "4. About",
      false,
  };
  update_result(&res, add_item(menu, (const struct MenuItem)first_item));
  update_result(&res, add_item(menu, (const struct MenuItem)second_item));
  update_result(&res, add_item(menu, (const struct MenuItem)third_item));
  update_result(&res, add_item(menu, (const struct MenuItem)forth_item));
  size_t idx = add_page(page_ctx, on_view_menu, false);
  update_result(&res, navigate_page(page_ctx, idx, menu, sizeof(*menu)));
  return CRESULT_SUCCESS;
}