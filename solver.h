#ifndef SOLVER_H
#define SOLVER_H

#include "io.h"
#include <stdbool.h>
#include <stdio.h>

#define FULL          (1)
#define EMPTY         (-1)

#define MAX_SIZE      (30)

typedef struct cell_struct {
  bool data; // 0/1 = empty / full
  bool enable; // 0/1 = can't / can write
} cell_t;

int solve(cell_t **prob_arr, int length, block_arr_t **block_arr,
                                         int blocks);
int solver_loop(int width, int height, cell_t *array[height][width],
               unsigned short int *blocks);
void print_grid(int width, int height, cell_t *grid[height][width]);

#endif
