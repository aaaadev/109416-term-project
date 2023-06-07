// SPDX-License-Identifier: MIT

#ifndef ABOUT_PAGE_H
#define ABOUT_PAGE_H

#include "../console/console.h"
#include "../console/page.h"
#include <stdlib.h>

// PUBLIC fields

extern enum ConsoleResult on_view_about_page(struct PageCtx *page_ctx,
                                             void *args);

#endif