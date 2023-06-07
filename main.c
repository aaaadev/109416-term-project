// SPDX-License-Identifier: MIT

#include "console/console.h"
#include "console/menu.h"
#include "console/page.h"
#include "console/dialog.h"
#include "pages/main_menu.h"
#include "constants.h"
#include "debug.h"
#include "main_signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const unsigned int DELAY_SECS = 1;

int main(void) {
  enum MainSignal *main_signal = getter_main_signal();
  pthread_mutex_t *signal_mutex = getter_signal_mutex();
  pthread_cond_t *signal_cond = getter_signal_cond();
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
    bool is_end_loop = false;
    pthread_mutex_lock(signal_mutex);
    while (*main_signal == UNKNOWN) {
      pthread_cond_wait(signal_cond, signal_mutex);
    }
    #ifdef DEBUG
      DPRINTF("signal?\n");
    #endif
    switch (*main_signal) {
      case QUIT:
        is_end_loop = true;
        break;
      default:
        break;
    }
    pthread_mutex_unlock(signal_mutex);
    if (is_end_loop) {
      #ifdef DEBUG
      DPRINTF("is_end_loop issued.\n");
      #endif
      break;
    }
  }
  console_shutdown(console);
  page_shutdown(page);
  return 0;
}