#include <stdio.h>

#ifndef IO_H
#define IO_H


int get_block_data(char *fileName, char *output);
int get_row_and_column_data(char *file_name, int *rows[30][14],
                            int *columns[30][14]);

#endif //IO_H
