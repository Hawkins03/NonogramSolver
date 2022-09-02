#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>

#define FULL          (1)
#define EMPTY         (-1)

#define MAX_SIZE      (30)

int solve(int *prob_arr[], int length, int block_arr[], int blocks);

#endif
