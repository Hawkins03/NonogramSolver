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

unsigned short int *get_block_data(char *file_name, int *height_ptr,
                                   int *width_ptr);
void print_grid(cell_t *grid[MAX_SIZE][MAX_SIZE], int width, int height);

#endif //IO_H
