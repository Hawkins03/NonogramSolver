#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define NULL_INPUT    (-2)
#define FILE_ERROR    (-3)
#define INPUT_ERROR   (-4)
#define MAX_SIZE      (30)

typedef struct block_arr_struct {
  int blocks;
  int max_block_len;
  unsigned short int *arr;
} block_arr_t;

void close_vars(FILE *in_file, block_arr_t *rows, block_arr_t *cols,
                       block_arr_t **blocks, int height, int width) {
block_arr_t **get_block_data(char *file_name, int *height_ptr,
                             int *width_ptr);

#endif //IO_H
