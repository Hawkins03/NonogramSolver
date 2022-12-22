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
  bool enable; // 0/1 = write enabled / write disabled (like registers)
} cell_t;

typedef struct row_struct {
  cell_t *row;
  unsigned short length;
} row_t;

int solver_loop(int width, int height, cell_t *array[height][width],
               block_arr_t **blocks);
#endif
