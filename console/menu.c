#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "console.h"
#include "menu.h"
#include "input.h"
#include "../debug.h"

struct MenuCtx* menu_init(struct ConsoleCtx *ctx, char * restrict name, on_ok_function on_ok) {
    struct MenuCtx *menu = malloc(sizeof (struct ConsoleCtx));
    menu->name = name;
    menu->ctx = ctx;
    menu->item_count = 0;
    menu->current_select = 0;
    menu->items = NULL;
    menu->on_ok = on_ok;
    return menu;
}

size_t add_item(struct MenuCtx *ctx, const struct MenuItem item) {
    ctx->items = realloc(ctx->items, sizeof(struct MenuItem)*(ctx->item_count+2));
    ctx->items[ctx->item_count] = (struct MenuItem)item;
    return (++ctx->item_count);
}

enum ConsoleResult show_menu(struct MenuCtx *ctx) {
    enum ConsoleResult result = CRESULT_SUCCESS;
    #ifdef DEBUG
    if (ctx->item_count == 0)
        DPRINTF("warning: ctx->item_count is zero, can cause undefined behavior\n");
        fflush(stdout);
    #endif
    ctx->items[ctx->current_select].is_selected = true;
    update_result(&result, clear_all(ctx->ctx));
    int lines = ctx->item_count + 1;
    int row = (ctx->ctx->window.window_size.ws_row - lines)/2;
    if (row < 1) {
        return CRESULT_TOOLONGCOUNT;
    }
    update_result(&result, simple_texth(ctx->ctx, strdup(ctx->name), row, HORIZONTAL_CENTER));
    row++;
    lines--;
    for (int i = row; i < row+lines; i++) {
        char *text = malloc(sizeof(char) * (strlen(ctx->items[i-row].text) + 10));
        text[0] = '\0';
        if (ctx->items[i-row].is_selected) {
            strcat(text, "> ");
        }
        strcat(text, ctx->items[i-row].text);
        update_result(&result, simple_texth(ctx->ctx, text, i, HORIZONTAL_CENTER));
    }
    set_cursor(ctx->ctx, (struct Cursor) { 0, 0 });
    
    // Key handle
    void **ret = malloc(sizeof(enum ConsoleResult));
    *ret = (void*)CRESULT_SUCCESS;
    pthread_t handler = handleInputArrow(ctx->ctx, key_event, ctx);
    pthread_join(handler, ret);
    update_result(&result, (enum ConsoleResult)(*ret));
    
    return result;
}

// PRIVATE
enum ConsoleResult key_event(char * const str, void *args) {
    char c = str[0];
    struct MenuCtx *ctx = args;
    switch(c) {
                    case 'A':
                        // UP
                        if (ctx->current_select != 0) {
                            ctx->items[ctx->current_select].is_selected = false;
                            ctx->current_select--;
                            ctx->items[ctx->current_select].is_selected = true;
                            update_menu(ctx);
                        }
                        break;
                    case 'B':
                        // DOWN
                        if (ctx->current_select != ctx->item_count-1) {
                            ctx->items[ctx->current_select].is_selected = false;
                            ctx->current_select++;
                            ctx->items[ctx->current_select].is_selected = true;
                            update_menu(ctx);
                        }
                        break;
                    case 'C':
                        // RIGHT
                        break;
                    case 'D':
                        // LEFT
                        break;
                    default:
                        break;
                }
    return CRESULT_SUCCESS;
}

enum ConsoleResult update_menu(struct MenuCtx *ctx) {
    enum ConsoleResult result = CRESULT_SUCCESS;
    #ifdef DEBUG
    if (ctx->item_count == 0)
        DPRINTF("warning: ctx->item_count is zero, can cause undefined behavior\n");
        fflush(stdout);
    #endif
    update_result(&result, clear_all(ctx->ctx));
    int lines = ctx->item_count + 1;
    int row = (ctx->ctx->window.window_size.ws_row - lines)/2;
    if (row < 1) {
        return CRESULT_TOOLONGCOUNT;
    }
    update_result(&result, simple_texth(ctx->ctx, strdup(ctx->name), row, HORIZONTAL_CENTER));
    row++;
    lines--;
    for (int i = row; i < row+lines; i++) {
        char *text = malloc(sizeof(char) * (strlen(ctx->items[i-row].text) + 10));
        text[0] = '\0';
        if (ctx->items[i-row].is_selected) {
            strcat(text, "> ");
        }
        strcat(text, ctx->items[i-row].text);
        update_result(&result, simple_texth(ctx->ctx, text, i, HORIZONTAL_CENTER));
    }
    set_cursor(ctx->ctx, (struct Cursor) { 0, 0 });
    return result;
}