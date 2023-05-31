#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include "console.h"

typedef void (*on_ok_function)(const struct Menu *);

struct MenuItem {
    char *text;
    bool is_selected;
};

struct MenuCtx {
    char *name;
    struct ConsoleCtx *ctx;
    size_t item_count;
    struct MenuItem *items;
    size_t current_select;
    on_ok_function on_ok;
};

// PUBLIC fields

extern struct MenuCtx* menu_init(struct ConsoleCtx *ctx, char * restrict name, on_ok_function on_ok);
extern size_t add_item(struct MenuCtx *ctx, const struct MenuItem item);
extern enum ConsoleResult show_menu(struct MenuCtx *ctx);
extern enum ConsoleResult update_menu(struct MenuCtx *ctx);

// PRIVATE fields

enum ConsoleResult key_event(char * const str, void *args);

#endif