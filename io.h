#ifndef IO_H
#define IO_H

#include "solver.h"
#include <stdio.h>

int get_block_data(char *file_name,
                   unsigned short *rows[MAX_SIZE][MAX_SIZE / 2],
                   unsigned short *columns[MAX_SIZE][MAX_SIZE / 2]);

#endif //IO_H
