// SPDX-License-Identifier: MIT

#include "../console/console.h"
#include "../console/menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const unsigned int DELAY_SECS = 5;

void on_ok_test(const struct Menu *menu) {
#ifdef DEBUG
  printf("on_ok_test() called\n");
  fflush(stdout);
#endif
  return;
}

int main(void) {
  struct ConsoleCtx *console = console_init();
  console_shutdown(console);
  return 0;
}