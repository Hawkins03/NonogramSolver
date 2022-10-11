#include "solver.h"

static void debug_row(cell_t **prob_arr, int front_offset, int end_offset,
                        int size, unsigned short int *block_arr, int blocks) {
  for (int i = 0; i < blocks; i++)
    printf("%hu ", block_arr[i]);
  printf("|");

  for (int i = 0; i < front_offset; i++)
    printf("-");
  for (int i = 0; i < size - front_offset; i++) {
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

  printf("|\n");
}

static int fill_cell(cell_t **A, int i, int d) {
  if ((!A) || (i >= d) || (i < 0)) return 0;
  if (!A[i]) return 0;
  if (!A[i]->enable) return 0;
  A[i]->data = 1;
  A[i]->enable = 0;
  return 1;
}

static int empty_cell(cell_t **A, int i, int d) {
  if ((!A) || (i >= d) || (i < 0)) return 0;
  if (!A[i]) return 0;
  if (!(A[i]->enable)) return 0;
  A[i]->data = 0;
  A[i]->enable = 0;
  return 1;
}

/*
 * solves a particular row:
 * prob_arr: the array of cells that the algorithm impliments over
 * length: the length of that array (used so that I can mess with it)
 * block_arr: the array of blocks/clues for that row
 * blocks: the number of blocks in the array (again, so I can mess with it)
 */
static int solve_row(cell_t **A, int n, unsigned short int *clue_ptr,
                     int clues) {
  int clue_num = 0;
  int empty_cells = 0;
  int full_cells = 0;

  int f = 0;
  int d = n;

  while (clue_num < clues) {
    if ((d - f < *clue_ptr + clue_num) || (!(clue_ptr + clue_num))) break;
    unsigned short int *clue = clue_ptr + clue_num;

    debug_row(A + f, f, 0, d, clue_ptr, clues);


    int offset = clues - 1 - clue_num;
    for (int i = clue_num; i < clues; i++)
      offset += *(clue_ptr + i);

    int min = d - offset;
    int max = *clue + f;

    if (f < 0)
      break;

    // loops ------------------------------------------------------------------
    for (int i = d - *clue; i < d; i++) {
      if (!((A[i]->data) || (A[i]->enable))) {
        d = i;
        continue;
      }
    }

    for (int i = f + *clue * 2 - 1; i >= f + *clue; i--) {
      if (i >= d) continue;
      if (A[i]->enable) continue;
      if (!A[i]->data) min = ((min < i) ? min : i);
    }

    /* TODO: fix this part. It's not working atm when f + clue = d
     * i.e. 9|.XXXXXXXX?|
     */
    if (clues - clue_num == 1) {
      for (int i = f + *clue - 1; i < d; i++) {
        if (A[i]->enable) continue;
        if (A[i]->data) max = ((max > i + 1) ? max : i + 1);
      }
      for (int i = d - 1; i >= f + *clue - 1; i--) {
        if (A[i]->enable) continue;
        if (A[i]->data) min = ((min < i) ? min : i);
      }
    }

    for (int i = f + *clue - 1; i >= f; i--) {
      if (A[i]->enable) continue;
      if (A[i]->data) min = ((min < i) ? min : i);
      else {
        printf("i: %d\n", i);
        if (f > 0)
          for (int j = f; j < i; j++)
            empty_cells += empty_cell(A, j, d);
        f = i + 1;
        continue;
      }
    }

    min = ((min > d - offset)?d - offset:min);

    printf("%hu: %d->%d\n", *clue, min, max);
    for (int i = min; i < max; i++)
      full_cells += fill_cell(A, i, d);

    if (max - min == *clue) {
      empty_cells += empty_cell(A, min - 1, d);
      empty_cells += empty_cell(A, max, d);
    }

    if (f > 0) {
      f += *clue + 1;
      clue_num++;
      continue;
    }

    int endcap = *clue - (max - min);
    endcap = ((endcap > 0) ? endcap:0);

    for (int i = f; i < min - endcap; i++)
      empty_cells += empty_cell(A, i, d);

    if (clues - clue_num == 1) {
      for (int i = max + endcap; i < d; i++)
        empty_cells += empty_cell(A, i, d);
    }

    f += *clue + 1;
    clue_num++;
  }

  return empty_cells + full_cells;
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
    debug_row(array[i], 0, 0, width, blocks[0][i].arr, blocks[0][i].blocks);
    printf("\n");
  }

  printf("\nSTARTING COLUMNS\n\n");
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++)
      prob_arr[j] = array[j][i];
    curr_sum += solve_row(prob_arr, height, blocks[1][i].arr,
                          blocks[1][i].blocks);
    debug_row(prob_arr, 0, 0, height, blocks[1][i].arr, blocks[1][i].blocks);
    printf("\n");
  }

  printf("Curr sum: %d\n", curr_sum);
  return curr_sum;
}

int main(int argc, char *argv[]) {// ------------------------------------------
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

