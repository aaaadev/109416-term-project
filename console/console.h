// SPDX-License-Identifier: MIT

#ifndef CONSOLE_H
#define CONSOLE_H

#include <termios.h>
#include <unistd.h>
#include <threads.h>
#include <sys/ioctl.h>

enum ConsoleResult {
    CRESULT_UNKNWON = 0xFF,

    CRESULT_SUCCESS = 0x00,
    CRESULT_FAILURE = 0x01,

    CRESULT_TOOLONGTEXT = 0x10,
    CRESULT_TOOLONGCOUNT = 0x11
};

static void update_result(enum ConsoleResult *lhs, enum ConsoleResult rhs) {
    if (((int)(*lhs) & 0xF0) < ((int)rhs & 0xF0)) {
        *lhs = rhs;
    }
    return;
}

enum TextAlignHorizontal {
    HORIZONTAL_LEFT = 0x00,
    HORIZONTAL_CENTER = 0x01,
    HORIZONTAL_RIGHT = 0x02,
};

enum TextAlignVertical {
    VERTICAL_TOP = 0xF0,
    VERTICAL_CENTER = 0xF1,
    VERTICAL_BOTTOM = 0xF2,
};

struct Cursor {
    int x;
    int y;
};

struct Window {
    char **data;
    struct winsize window_size;
};

struct ConsoleCtx {
    struct Cursor cursor;
    mtx_t window_mutex;
    struct Window window;
    struct termios current;
    struct termios original_termios;
};

// PUBLIC fields
extern struct ConsoleCtx* console_init(void);
extern enum ConsoleResult console_shutdown(struct ConsoleCtx *ctx);
extern enum ConsoleResult refresh_window(struct ConsoleCtx *ctx);
extern enum ConsoleResult set_cursor(struct ConsoleCtx *ctx, struct Cursor cur);
extern enum ConsoleResult raw_sync_cursor(struct ConsoleCtx *ctx);
extern enum ConsoleResult clear_line(struct ConsoleCtx *ctx, int row);
extern enum ConsoleResult clear_all(struct ConsoleCtx *ctx);

extern enum ConsoleResult basic_draw(struct ConsoleCtx *ctx);
extern enum ConsoleResult basic_text(struct ConsoleCtx *ctx, struct Cursor cur, char * restrict text);

extern enum ConsoleResult simple_text(struct ConsoleCtx *ctx, char * restrict text);
extern enum ConsoleResult simple_textvh(struct ConsoleCtx *ctx, char * restrict text, enum TextAlignHorizontal align_h, enum TextAlignVertical align_v);
extern enum ConsoleResult simple_texth(struct ConsoleCtx *ctx, char * restrict text, int row, enum TextAlignHorizontal align_h);

// PRIVATE fields
void enable_raw_mode(struct ConsoleCtx *ctx);
void disable_raw_mode(struct ConsoleCtx *ctx);
void raw_clear_console(void);
void raw_set_cursor(int x, int y);

#endif