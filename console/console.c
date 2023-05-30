// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <threads.h>
#include "console.h"
#include "../debug.h"

struct ConsoleCtx* console_init(void) {
    struct ConsoleCtx *ctx = malloc(sizeof(struct ConsoleCtx));
    mtx_init(&ctx->window_mutex, mtx_plain);
    enable_raw_mode(ctx);
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ctx->window.window_size);
    #ifdef DEBUG
    DPRINTF("row: %d col: %d\n", ctx->window.window_size.ws_row, ctx->window.window_size.ws_col);
    fflush(stdout);
    #endif
    ctx->window.data = (char **)malloc(sizeof(char *) * (ctx->window.window_size.ws_row+1));
    for (int i = 0; i < ctx->window.window_size.ws_row; i++) {
        ctx->window.data[i] = (char *)malloc(sizeof(char) * (ctx->window.window_size.ws_col+1));
    }
    basic_draw(ctx);
    refresh_window(ctx);
    // MUST BE FREED LATER
    return ctx;
}

enum ConsoleResult sync_cursor(struct ConsoleCtx *ctx) {
    printf("\033[6n");
    fflush(stdout);
    
    char buf[32];
    int ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    if (ret > 0) {
        buf[ret] = '\0';
        sscanf(buf, "\033[%d;%dR", &ctx->cursor.x, &ctx->cursor.y);
    }
    
    // Handle 1-indexed
    ctx->cursor.x--;
    ctx->cursor.y--;

    return CRESULT_SUCCESS;
}

enum ConsoleResult basic_draw(struct ConsoleCtx *ctx) {
    for (int i = 0; i < ctx->window.window_size.ws_col; i++) {
        ctx->window.data[0][i] = '#';
    }
    for (int i = 1; i < ctx->window.window_size.ws_row - 1; i++) {
        ctx->window.data[i][0] = '#';
        for (int j = 1; j < ctx->window.window_size.ws_col - 1; j++) {
            ctx->window.data[i][j] = ' ';
        }
        ctx->window.data[i][ctx->window.window_size.ws_col - 1] = '#';
    }
    for (int i = 0; i < ctx->window.window_size.ws_col; i++) {
        ctx->window.data[ctx->window.window_size.ws_row - 1][i] = '#';
    }
    return CRESULT_SUCCESS;
}

enum ConsoleResult refresh_window(struct ConsoleCtx *ctx) {
    enum ConsoleResult result = CRESULT_SUCCESS;
    update_result(&result, set_cursor(ctx, (struct Cursor) { 0, 0 }));
    raw_clear_console();
    for (int i = 0; i < ctx->window.window_size.ws_row; i++) {
        for (int j = 0; j < ctx->window.window_size.ws_col; j++) {
            printf("%c", ctx->window.data[i][j]);
        }
        if (i != ctx->window.window_size.ws_row-1)
            printf("\n");
    }
    fflush(stdout);
    update_result(&result, set_cursor(ctx, (struct Cursor) { 0, 0 }));
    return result;
}

enum ConsoleResult console_shutdown(struct ConsoleCtx * restrict ctx) {
    disable_raw_mode(ctx);
    mtx_destroy(ctx->window_mutex)
    free(ctx);
    return CRESULT_SUCCESS;
}

enum ConsoleResult set_cursor(struct ConsoleCtx *ctx, struct Cursor cur) {
    ctx->cursor = cur;
    raw_set_cursor(cur.x, cur.y);
    return CRESULT_SUCCESS;
}

enum ConsoleResult basic_text(struct ConsoleCtx *ctx, struct Cursor cur, char * restrict text) {
    set_cursor(ctx, cur);
    int end_y = cur.y + strlen(text);
    if (end_y > ctx->window.window_size.ws_col-1) {
        return CRESULT_TOOLONGTEXT;
    } else {
        for (int i = cur.y; i < end_y; i++) {
            ctx->window.data[cur.x][i] = text[i-cur.y];
            printf("%c", ctx->window.data[cur.x][i]);
        }
        fflush(stdout);
        sync_cursor(ctx);
        return CRESULT_SUCCESS;
    }
}

enum ConsoleResult simple_textvh(struct ConsoleCtx *ctx, char * restrict text, enum TextAlignHorizontal align_h, enum TextAlignVertical align_v) {
    enum ConsoleResult result = CRESULT_SUCCESS;
    int len = strlen(text);
    int col = ctx->window.window_size.ws_col;
    int lines = (len-1)/(col-2) + 1;
    int last_idx = 0;
    switch(align_v) {
        case VERTICAL_TOP:
            last_idx = 1 + lines - 1;
            for (int i = 1; i <= last_idx; i++) {
                char *partial_text = NULL;
                if (last_idx == i) {
                    partial_text = malloc((len % (col-2) + 1) * sizeof(char));
                    strncpy(partial_text, text, len % (col-2));
                    partial_text[len % (col-2)] = '\0';
                } else {
                    partial_text = malloc((col-1) * sizeof(char));
                    strncpy(partial_text, text, col-2);
                    text += col-2;
                    partial_text[col-2] = '\0';
                }
                update_result(&result, simple_texth(ctx, partial_text, i, align_h));
            }
            break;
        case VERTICAL_CENTER:
            last_idx = (ctx->window.window_size.ws_row-lines)/2 + lines - 1;
            for (int i = (ctx->window.window_size.ws_row-lines)/2; i <= last_idx; i++) {
                char *partial_text = NULL;
                if (last_idx == i) {
                    partial_text = malloc((len % (col-2) + 1) * sizeof(char));
                    strncpy(partial_text, text, len % (col-2));
                    partial_text[len % (col-2)] = '\0';
                } else {
                    partial_text = malloc((col-1) * sizeof(char));
                    strncpy(partial_text, text, col-2);
                    text += col-2;
                    partial_text[col-2] = '\0';
                }
                update_result(&result, simple_texth(ctx, partial_text, i, align_h));
            }
            break;
        default:
            #ifdef DEBUG
            DPRINTF("undefined case reached\n");
            fflush(stdout);
            #endif
            break;
    }
    return result;
}

enum ConsoleResult simple_texth(struct ConsoleCtx *ctx, char * restrict text, int row, enum TextAlignHorizontal align_h) {
    enum ConsoleResult result = CRESULT_SUCCESS;
    int len = strlen(text);
    switch(align_h) {
        case HORIZONTAL_LEFT:
            update_result(&result, basic_text(ctx, (struct Cursor) { row, 1 }, text));
            break;
        case HORIZONTAL_CENTER:
            if (len > ctx->window.window_size.ws_col-2) {
                update_result(&result, CRESULT_TOOLONGTEXT);
            } else {
                update_result(&result, basic_text(ctx, (struct Cursor) { row, (ctx->window.window_size.ws_col-len)/2 }, text));
            }
            break;
        case HORIZONTAL_RIGHT:
            if (len > ctx->window.window_size.ws_col-2) {
                update_result(&result, CRESULT_TOOLONGTEXT);
            } else {
                update_result(&result, basic_text(ctx, (struct Cursor) { row, ctx->window.window_size.ws_col-1-len }, text));
            }
            break;
        default:
            break;
    }
    return CRESULT_SUCCESS;
}

enum ConsoleResult clear_line(struct ConsoleCtx *ctx, int row) {
    int col = ctx->window.window_size.ws_col;
    set_cursor(ctx, (struct Cursor) { row, 1 });
    for (int i = 1; i < col-1; i++) {
        printf(" ");
    }
    sync_cursor(ctx);
    fflush(stdout);
    return CRESULT_SUCCESS;
}

enum ConsoleResult clear_all(struct ConsoleCtx *ctx) {
    enum ConsoleResult result = CRESULT_SUCCESS;
    for (int i = 1; i < ctx->window.window_size.ws_row-1; i++) {
        update_result(&result, clear_line(ctx, i));
    }
    return result;
}

// PRIVATE
void raw_clear_console() {
    printf("\033[2J\033[H");
    return;
}

// PRIVATE
void raw_set_cursor(int x, int y) {
    printf("\033[%d;%dH", x + 1, y + 1);
}

// PRIVATE
void enable_raw_mode(struct ConsoleCtx *ctx) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &ctx->original_termios);
    raw = ctx->original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    ctx->current = raw;
    return;
}

// PRIVATE
void disable_raw_mode(struct ConsoleCtx *ctx) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->original_termios);
    return;
}