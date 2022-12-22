#include "solver.h"


static void print_row(cell_t **prob_arr, int front_offset, int end_offset,
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

/**
 * solves a particular row:
 * prob_arr: the array of cells that the algorithm impliments over
 * length: the length of that array (used so that I can mess with it)
 * block_arr: the array of blocks/clues for that row
 * blocks: the number of blocks in the array (again, so I can mess with it)
 *
 * Solving Process:
 *  Per Clue:
 *    1. Calculate offset and set barriers
 *    2. itteratevly shrink those barriers
 *      a. if max - min = 0 then end the loop
 *      b. if there's a full cell in the range [min, min + *clue] it's the clue
 *       (shrink search area)
 *      c. if there's an empty cell in the range [min, min + *clue) or
 *       (max - *clue, max] shrink search area
 *    3. fill [min, max]
 *      a. add endcaps if max-min = *clue
 *    4. if # clues = 1, empty all unreachable spaces. (then exit solve_row)
 *      a. if there are more clues, then only do so for cells before, not after
 *    5. go to the next block
 *      a. if max-min = *clue, f += max + 2
 *      b. else, f = f + *clue + 1
 *
 * Notes:
 * f, min are [ (at index, so use >=)
 * d, max are ) (1 beyond end, so use <)
 */
static int solve_row(cell_t **A, int n, unsigned short int *clue_ptr,
                     int clues) {
  int clue_num = 0;
  int empty_cells = 0;
  int full_cells = 0;

  int f = 0;
  int d = n;

  // step 4 (early)
  for (int i = d - *clue_ptr + clues - 1; i < d; i++) {
    if (!((A[i]->data) || (A[i]->enable))) { // not X, not write enabled
      for (int j = i; i < d; i++)
        empty_cell(A, j, d);
      return solve_row(A, i, clue_ptr, clues);
    }
  }


  while (clue_num < clues) {    // per clue
    // TODO: Check the edge cases on this line \/
    if ((d - f < *clue_ptr + clue_num) || (!(clue_ptr + clue_num))) break;

    // printing row
    unsigned short int *clue = clue_ptr + clue_num;
    print_row(A + f, f, 0, d, clue_ptr, clues);

    // calculating offset
    int offset = clues - 1 - clue_num;
    for (int i = clue_num; i < clues; i++)
      offset += *(clue_ptr + i);

    // marking upper and lower bounds for the search
    int upper = d - offset;
    int lower = f;

    // clue must be in [min,max)
    int min = d - offset;
    int max = *clue + f;

    if (f < 0) // Insanity checking
      break;

    bool repeat = false;
    // 2. itterativly shrink said barriars (all of the following will need to
    //    be updated)
    do {
      bool updated = false;

      // a. check for full cells from [f, f + *clue]
      for (int i = lower + *clue; i >= lower; i--) {
        if ((A[i]->data) && (!(A[i]->enable))) {
          repeat = true;
          min = i;
          if (!updated) {
            max = i;
            upper = i + *clue;
            updated = true;
          }
        }
      }

      // b. check for empty cells from [f, f + *clue)
      for (int i = f + *clue - 1; i >= f; i--) {
        if (!((A[i]->data) || (A[i]->enable))) {
          repeat = true;
          lower += i;
          // update min / max
          break;
        }
      }

      // c. check for empty cells from (min - *clue, min]
      for (int i = upper - *clue; i < upper; i++) {
        if (!((A[i]->data) || (A[i]->enable))) {
          upper = i;
          // update min / max
          repeat = true;
          break;
        }
      }
    } while (repeat);

    // step 3.
    printf("%hu: %d->%d\n", *clue, min, max);
    for (int i = min; i < max; i++)
      full_cells += fill_cell(A, i, d);

    // step 3a. add endcaps
    if (max - min == *clue) {
      empty_cells += empty_cell(A, min - 1, d);
      empty_cells += empty_cell(A, max, d);
    }

    // step 4. (don't touch for now)

    /* TODO: fix this part. It's not working atm when f + clue = d
     * i.e. 9|.XXXXXXXX?|
     * EDIT: I don't think it's this one
     */
    if (clues - clue_num == 1) {
      for (int i = f + *clue; i < d; i++) {
        if (A[i]->enable) continue;
        if (A[i]->data) max = ((max > i + 1) ? max : i + 1);
      }
      for (int i = d - 1; i >= f + *clue - 1; i--) {
        if (A[i]->enable) continue;
        if (A[i]->data) min = ((min < i) ? min : i);
      }
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

    // step 5.
    f += *clue + 1;
    clue_num++;
  }

  return empty_cells + full_cells;
}

/**
 * main loop for solving stuff - solves each row and then column.
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
    print_row(array[i], 0, 0, width, blocks[0][i].arr, blocks[0][i].blocks);
    printf("\n");
  }

  printf("\nSTARTING COLUMNS\n\n");
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++)
      prob_arr[j] = array[j][i];
    curr_sum += solve_row(prob_arr, height, blocks[1][i].arr,
                          blocks[1][i].blocks);
    print_row(prob_arr, 0, 0, height, blocks[1][i].arr, blocks[1][i].blocks);
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


