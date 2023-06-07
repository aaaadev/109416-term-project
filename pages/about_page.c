// SPDX-License-Identifier: MIT

#include "about_page.h"
#include "../console/console.h"
#include "../console/dialog.h"
#include "../console/page.h"
#include <stdlib.h>

enum ConsoleResult on_view_about_page(struct PageCtx *page_ctx, void *args) {
  // about dummy page
  enum ConsoleResult res = CRESULT_SUCCESS;
  struct ConsoleCtx *ctx = page_ctx->ctx;
  struct OkDialogConfig cfg = {
      "About",
      "Version: 0.1.0\nLicense: MIT\nGithub : aaaadev/109416-term-project",
  };
  size_t idx = add_page(page_ctx, on_view_ok_dialog, false);
  update_result(&res, navigate_page(page_ctx, idx, (void *)&cfg, sizeof(cfg)));
  return res;
}