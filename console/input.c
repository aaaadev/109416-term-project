// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "console.h"
#include "input.h"
#include "../debug.h"

pthread_t handleInputArrow(struct ConsoleCtx *ctx, handle_key func, void *inner_args) {
    pthread_t handler;
    struct InputCtx *args = malloc(sizeof(struct InputCtx));
    args->ctx = ctx;
    args->func = func;
    args->count = 1;
    args->options = DEFAULT;
    args->args = inner_args;
    pthread_create(handler, NULL, handleInputArrowInner, (void*)args);
    return handler;
}

pthread_t handleInputLine(struct ConsoleCtx *ctx, size_t count, handle_key func, void *inner_args) {
    pthread_t handler;
    struct InputCtx *args = malloc(sizeof(struct InputCtx));
    args->ctx = ctx;
    args->func = func;
    args->count = count;
    args->options = DEFAULT;
    args->args = inner_args;
    pthread_create(handler, NULL, handleInputLineInner, (void*)args);
    return handler;
}

// PRIVATE

void handleInputArrowInner(void *args) {
    struct InputCtx ctx = *((struct InputCtx*)(args));
    char c = '\0';
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\033') {
            read(STDIN_FILENO, &c, 1);
            if (c == '[') {
                read(STDIN_FILENO, &c, 1);
                char *str = malloc(sizeof(char)*2);
                str[0] = c;
                str[1] = '\0';
                ctx.func(str, ctx.args);
            }
        } else {
            #ifdef DEBUG
            DPRINTF("unknown key detected\n");
            fflush(stdout);
            #endif
        }
    }
    enum ConsoleResult res = CRESULT_SUCCESS;
    pthread_exit((void*)res);
}

// PRIVATE

void handleInputLineInner(void *args) {
    struct InputCtx ctx = *((struct InputCtx*)(args));
    char *input = malloc(sizeof(char) * ctx.count);
    if (fgets(input, sizeof(input), stdin) != NULL) {
        char *newline = strchr(input, '\n');
        if (newline != NULL) {
            *newline = '\0';
            ctx.func(input, ctx.args);
        } else {
            #ifdef DEBUG
            printf("error: buffer size overflow occurred. expected size: %ld\n", ctx.count);
            fflush(stdout);
            #endif
            enum ConsoleResult res = CRESULT_FAILURE;
            pthread_exit((void*)res);
        }
    }
    enum ConsoleResult res = CRESULT_SUCCESS;
    pthread_exit((void*)res);
}