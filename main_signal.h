// SPDX-License-Identifier: MIT

#ifndef MAIN_SIGNAL_H
#define MAIN_SIGNAL_H

#include <pthread.h>

enum MainSignal {
    UNKNOWN,
    QUIT,
};

extern enum MainSignal* getter_main_signal();
extern pthread_mutex_t* getter_signal_mutex();
pthread_cond_t* getter_signal_cond();

#endif