#include "main_signal.h"
#include <pthread.h>

static enum MainSignal main_signal = UNKNOWN;
static pthread_mutex_t signal_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t signal_cond = PTHREAD_COND_INITIALIZER;

enum MainSignal* getter_main_signal() {
    return &main_signal;
}

pthread_mutex_t* getter_signal_mutex() {
    return &signal_mutex;
}

pthread_cond_t* getter_signal_cond() {
    return &signal_cond;
}