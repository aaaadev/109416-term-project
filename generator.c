// SPDX-License-Identifier: MIT

#include "util/generate_numbers.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    size_t cnt = 0;
    char *filename = malloc(sizeof(char) * 4096);
    if ((argc != 3) || (sscanf(argv[1], "%zu", &cnt) != 1) || (sscanf(argv[2], "%s", filename) != 1)) {
        fprintf(stderr, "Invalid input.\n");
        fflush(stderr);
    } else {
        FILE *file = fopen(filename, "wx");
        if (file == NULL) {
            fprintf(stderr, "File already exists.\n");
            fflush(stderr);
        } else {
            for (size_t i = 0; i < cnt; i++) {
                uint64_t *num = generate_numbers();
                fprintf(file, "%llu %llu %llu %llu %llu %llu\n", num[0], num[1], num[2], num[3], num[4], num[5]);
            }
            fflush(file);
            fclose(file);
            printf("Ok, %zu lines are generated.\n", cnt);
            fflush(stdout);
        }
    }
    return 0;
}