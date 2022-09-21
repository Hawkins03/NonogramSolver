#include "solver.h"

static int fill_cell(cell_t *cell) {
  if (!cell)
    return 0;
  if (!cell->enable)
    return 0;
  cell->data = 1;
  cell->enable = 0;
  return 1;
}

static int empty_cell(cell_t *cell) {
  if (!cell)
    return 0;
  if (!(cell->enable))
    return 0;
  cell->data = 0;
  cell->enable = 0;
  return 1;
}

/*
 * solves a particular row:
 * prob_arr: the array of cells that the algorithm impliments over
 * length: the length of that array (used so that I can mess with it)
 * block_arr: the array of blocks/clues for that row
 * blocks: the number of blocks in the array (again, so I can mess with it)
 *
 * how it works:
 * 1. ensure everything exists (more an insanity check than an actual one)
 * 2. find the total block length
 * 3. for the first block:
 *    a. find where it HAS to be
 *    b. fill that space
 * 4. go to the next block
 */
static int solve_row(cell_t **prob_arr, int length,
                     unsigned short int *block_arr, int blocks) {
  int num_found = 0;

  //insanity check
  if ((blocks == 0) || (*block_arr == 0)) {
    for (int i = 0; i < length; i++)
      num_found += empty_cell(prob_arr[i]);
    return num_found;
  }

  // calculating total_block_length
  int total_block_len = blocks - 1;

  for (int i = 0; i < blocks; i++)
    total_block_len += block_arr[i];

  if (length < total_block_len)
    return num_found;

  // main body
  int max_index = 0;
  int min_index = length - total_block_len;


  // starting from max going to min shortening the thingey
  for (int i = max_index; i > min_index + *block_arr; i--) {
    if (prob_arr[i]->enable)
      continue;

    if (prob_arr[i]->data)
      max_index = i + *block_arr;

    else {
      for (int j = 0; j < i; j++)
        num_found += empty_cell(prob_arr[j]);

      min_index = i;
      return num_found;
    }
  }

  // filling array
  for (int i = min_index; i < max_index + *block_arr; i++)
    num_found += fill_cell(prob_arr[i]);

  if ((max_index - min_index == 0) && ((min_index + *block_arr + 1) < length))
    empty_cell(prob_arr[min_index + *block_arr]);

 if (blocks == 1)
    return num_found;

  num_found += solve_row(prob_arr + *block_arr + 1, length - *block_arr - 1,
                     block_arr + 1, blocks - 1);
  return num_found;
}

int solver_loop(int width, int height, cell_t *array[height][width],
                block_arr_t **blocks) {
  cell_t *prob_arr[width];
  int curr_sum = 0;

  //printf("\nSTARTING ROWS\n\n");
  for (int i = 0; i < height; i++)
    curr_sum += solve_row(array[i], width, blocks[0][i].arr,
                          blocks[0][i].blocks);
  //debug_row(array[i], 0, 0, width, blocks[0][i].arr, blocks[0][i].blocks, 0);


  //printf("\nSTARTING COLUMNS\n\n");
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++)
      prob_arr[j] = array[i][j];
    curr_sum += solve_row(prob_arr, height, blocks[1][i].arr,
                          blocks[1][i].blocks);
  }
  //debug_row(array[i], 0, 0, height, blocks[1][i].arr, blocks[1][i].blocks, 0);

  return curr_sum;
}

int main(int argc, char *argv[]) {
  int width, height;

  char *file_name = "blocks.txt";

  block_arr_t **blocks = get_block_data(file_name, &height, &width);
  if (!blocks)
    return -1;
  //printf("width: %d height, %d\n", width, height);

  //non-temp variables
  cell_t array_setup[width][height]; // make setup_array thingey.
  cell_t *array[width][height];

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      array[i][j] = &array_setup[i][j];
      array_setup[i][j].data = 0;
      array_setup[i][j].enable = 1;
    }
  }

  int curr_sum = 1;
  while (curr_sum > 0) {
    curr_sum = solver_loop(width, height, array, blocks);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (array[i][j]->enable) {
        printf("?");
        continue;
      }
      if (array[i][j]->data)
        printf("O");
      else
        printf("X");
    }
    printf("\n");
  }

  return 0;
}

/*
static void debug_row(cell_t **prob_arr, int front_offset, int end_offset,
                        int size, unsigned short int *block_arr, int blocks,
                                                              int branch_num) {
  for (int i = 0; i < blocks; i++)
    printf("%hu ", block_arr[i]);
  printf("|");

  for (int i = 0; i < front_offset; i++)
    printf("-");
  for (int i = 0; i < size; i++) {
    if (prob_arr[i]->enable) {
      printf("?");
    }
    else {
      if (prob_arr[i]->data)
        printf("X");
      else
        printf(".");
    }
  }

  for (int i = 0; i < end_offset; i++)
    printf("-");

  printf("| (%d)\n", branch_num);
} */

