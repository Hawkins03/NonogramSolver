#include "solver.h"

static void debug_row(cell_t **prob_arr, int front_offset, int end_offset,
                        int size, unsigned short int *block_arr, int blocks,
                                                              int branch_num) {
  for (int i = 0; i < blocks; i++)
    printf("%hu ", block_arr[i]);
  printf("|");

  for (int i = 0; i < front_offset; i++)
    printf("-");
  for (int i = 0; i < size; i++) {
    if (prob_arr[i]->enable)
      printf("?");
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
}

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
static int solve_row(cell_t **A, int n,
                     unsigned short int *clue, int clues) {
  // setup---------------------------------------------------------------------
  if ((n <= 0) || (clues == 0))
    return 0;

  debug_row(A, 0, 0, n, clue, clues, 0);

  int offset = clues - 1;
  for (int i = 0; i < clues; i++)
    offset += *(clue + i);

  int f = n - offset;
  int d = *clue;
  int num_found = 0;

  if (f < 0)
    return 0;


  // loops---------------------------------------------------------------------
  // //TODO add way to check different possibilities

  // extend the range we look for f
  if (clues == 1) {
    for (int i = f; i >= d; i--) {
      if (A[i]->enable) continue;
      if (A[i]->data) f = i;
    }

    for (int i = d; i < n; i++) {
      if (A[i]->enable) continue;
      if (A[i]->data) d = i + 1;
    }
  }

  // use filled cells to find the front (in all cases)
  for (int i = *clue - 1; i >= 0; i--) {
    if (A[i]->enable) continue;

    if (A[i]->data)
      f = i;
    else {
      for (int j = 0; j < i; j++)
        num_found += empty_cell(A[j]);
      return num_found + solve_row(A + i + 1, n - i - 1, clue, clues);
    }
  }

  for (int i = n - 1; i > n - *clue; i--) {
    if (A[i]->enable) continue;
    if (!A[i]->data)
      return num_found + solve_row(A, n - i, clue, clues);
  }

  //filling spaces
  printf("%hu: %d->%d\n", *clue, f, d);
  for (int i = f; i < d; i++)
    num_found += fill_cell(A[i]);

  // filling spaces where it can't be.
  int endcap = *clue - d + f;
  for (int i = 0; i < f - endcap; i++)
    num_found += empty_cell(A[i]);

  if (clues == 1) {
    for (int i = d + endcap; i < n; i++)
      num_found += empty_cell(A[i]);
    return num_found;
  }


  num_found += solve_row(A + (*clue) + 1, n - (*clue) - 1, clue + 1, clues - 1);

  if (d - f == *clue)
    num_found += empty_cell(A[f + *clue]);

  return num_found;
}

/*
 * main loop for solving stuff - solves each row and then column.
 * (debug-row is just a print call, it helps with debugging)
 *
 */
int solver_loop(int width, int height, cell_t *array[height][width],
                block_arr_t **blocks) {
  cell_t *prob_arr[width];
  int curr_sum = 0;

  printf("\nSTARTING ROWS\n\n");
  for (int i = 0; i < height; i++) {
    curr_sum += solve_row(array[i], width, blocks[0][i].arr,
                          blocks[0][i].blocks);
    debug_row(array[i], 0, 0, width, blocks[0][i].arr, blocks[0][i].blocks, 1);
    printf("\n");
  }

  printf("\nSTARTING COLUMNS\n\n");
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++)
      prob_arr[j] = array[j][i];
    curr_sum += solve_row(prob_arr, height, blocks[1][i].arr,
                          blocks[1][i].blocks);
    debug_row(prob_arr, 0, 0, height, blocks[1][i].arr, blocks[1][i].blocks, 1);
    printf("\n");
  }

  printf("Curr sum: %d\n", curr_sum);
  return curr_sum;
}

int main(int argc, char *argv[]) {
  int width, height;

  char *file_name = "blocks.txt";

  block_arr_t **blocks = get_block_data(file_name, &height, &width);
  if (!blocks)
    return -1;
  printf("width: %d height, %d\n", width, height);

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
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        if (array[i][j]->enable) {
          printf("?");
          continue;
        }
        if (array[i][j]->data)
          printf("X");
        else
          printf(".");
      }
      printf("\n");
    }
  }



  return 0;
}

/*
*/

