#ifndef IO_H
#define IO_H

#include "solver.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define NULL_INPUT    (-2)
#define FILE_ERROR    (-3)
#define INPUT_ERROR   (-4)

typedef unsigned short int clue_arr[MAX_SIZE][MAX_SIZE/2];

typedef struct clue_struct {
  unsigned short int rows[MAX_SIZE][MAX_SIZE / 2];
  unsigned short int columns[MAX_SIZE][MAX_SIZE / 2];
} clue_storage_t;

clue_storage_t *get_block_data(char *file_name);

#endif //IO_H
