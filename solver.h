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

int solve(cell_t *prob_arr[], int length, int block_arr[], int blocks);
void print_grid(cell_t *grid[MAX_SIZE][MAX_SIZE], int width, int height);

#endif
