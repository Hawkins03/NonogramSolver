#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>

#define FULL          (1)
#define EMPTY         (-1)

#define MAX_SIZE      (30)

typedef struct cell_struct {
  bool full;
  bool empty;
} cell_t;

int solve(cell_t *prob_arr[], int length, int block_arr[], int blocks);

#endif
