#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>

#define FULL          (-2)
#define EMPTY         (-3)

#define MAX_SIZE      (30)

int mark_empty(int **index_ptr);
int mark_full(int **index_ptr);
int check_full(int *prob_arr[], int length, int block_arr[], int blocks);
int solve(int *prob_arr[], int length, int block_arr[], int blocks);

#endif
