#ifndef IO_H
#define IO_H

#include "solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define NULL_INPUT    (-2)
#define FILE_ERROR    (-3)
#define INPUT_ERROR   (-4)

int ***get_block_data(char *file_name);

#endif //IO_H
