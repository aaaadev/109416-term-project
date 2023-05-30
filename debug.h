// SPDX-License-Identifier: MIT

#ifndef DEBUG_H
#define DEBUG_H

static const char *DEBUG_TAG = "DEBUG";

#define DPRINTF(format, ...) printf("[%s] " format, DEBUG_TAG, ##__VA_ARGS__)

#endif