#include "solver.h"


static void print_row(cell_t **prob_arr, int f, int size,
                      unsigned short int *block_arr, int blocks) {
  if (f < 0) f = 0;
  for (int i = 0; i < blocks; i++)
    printf("%hu ", block_arr[i]);
  printf("|");

  for (int i = 0; i < f; i++)
    printf("-");
  for (int i = f; i < size; i++) {
    if (prob_arr[i]->enable)
      printf("?");
    else {
      if (prob_arr[i]->data)
        printf("X");
      else
        printf(".");
    }
  }

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
 * TODO: add in check that if max = d or max + 1 is empty then min = max - *clue
 *       and same for min (but in reverse)
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
 *
 *
 * TODO: Need to solidify the recursion otherwise it could cause an infinite
 * loop. I believe a good solution would be to impliment an array at the clue
 * level. This could be used to remove non-viable locations and could be used
 * to make things much easier.
 *
 * TODO: find some actual test cases to test against insted of running it again
 * and again.
 */
static int solve_row(cell_t **A, int d, unsigned short int *clue_ptr,
                     int clues) {
  if (d == 0) return 0;

  int clue_num = 0;
  int empty_cells = 0;
  int full_cells = 0;

  int f = 0;

  print_row(A, 0, d, clue_ptr, clues);

  while (clue_num < clues) {
    bool searched[d]; // holds viable search locations (if full or empty, not viable.)

    unsigned short int *clue = clue_ptr + clue_num;
    //printf("clue: %d ", *clue);

    // insanity check
    // TODO: double check clue is set up correctly and I didn't break anything
    if ((d - f < *clue) || (!clue)) break;

    // calculating offset
    int offset = clues - 1 - clue_num;
    for (int i = clue_num; i < clues; i++)
      offset += *(clue_ptr + i);

    //printf("Offset: %d\n", offset);

    // marking upper and lower bounds for the search
    // TODO: finish
    int upper = d - offset;
    int alt_upper = (f + 2 * *clue + 1 < d)? (f + 2 * *clue + 1) : (f);
    upper = (upper > alt_upper) ? upper : alt_upper;
    int lower = f;

    for (int i = 0; i < d; i++) // TODO: read from upper + *clue to influence
      searched[i] = true;       // regardless of viableness. (or edit viable
                                // bounds to better scan)
    for (int i = lower; i < upper; i++)
      searched[i] = false;

    // clue must be in [min,max)
    int min = d - offset;
    int max = *clue + f;
    printf("offset = %d, u/l = %d/%d\n", offset, upper, lower);

    if (f < 0) // Insanity checking
      break;

    //printf("Lower: %d, Upper: %d, min: %d, max: %d, ", lower, upper, min, max);
    // printing row
    print_row(A, f, d, clue_ptr, clues);

    bool repeat;
    // 2. itterativly shrink said barriars (all of the following will need to
    //    be updated)
    // TODO: vigerously check that this works
    //printf("optimizing search locations:\n");
    do {
      //printf("Upper = %d, Lower = %d\n", upper, lower);
      printf("Searched: ");
      for (int i = 0; i < d; i++) {
        if (searched[i]) printf("Y");
        else printf("N");
      }
      printf("\n");
      repeat = false;

      // b. check for empty cells from [lower, lower + *clue)
      for (int i = ((lower + *clue < d) ? lower + *clue : d); i >= lower; i--) {
        if (searched[i]) continue;
        if (!((A[i]->data) || (A[i]->enable))) { // empty cell
          //TODO: make function to check full / empty cells to prevent SEGV
          //printf("loop 1: found empty cell @ index %d\n", i);
          searched[i] = true;
          repeat = true;
          max -= i - lower + 1;
          lower = i + 1;
          break;
        }
      }

      // c. check for empty cells from (upper - *clue, upper]
      for (int i = ((upper - *clue > 0) ? (upper - *clue): 0); i < upper; i++) {
        if (searched[i]) continue;
        if (!((A[i]->data) || (A[i]->enable))) { // empty cell
          //printf("loop 2: found empty cell @ index %d\n", i);
          searched[i] = true;
          repeat = true;
          min -= upper - i;
          upper = i;
          break;
        }
      }

      // last bc it trumps steps b & c, reminder that loop over same thing
      // a. check for full cells from [f, f + *clue]
      //
      // TODO: fix. It's not detecting / finalizing stuffs.
      // need base case.
      // need to search the surrouning cells for additional full cells
      for (int i = (lower + *clue < d) ? (lower + *clue) : lower - 1;
           i >= lower; i--) {
        if (searched[i]) continue;
        if ((A[i]->data) && (!(A[i]->enable))) { //TODO: fix this. might need upper loop looking for full cells.
          //printf("loop 3: found full cell @ index %d\n", i);
          int ittr = i;
          while ((ittr < d - 1) && (A[ittr + 1]->data && (!A[i]->enable))) {
            ittr++;
          }
          searched[i] = true;
          repeat = true;
          min = i;
        }
      }
      for (int i = lower; (i <= lower + *clue) && (i < d); i++) {
        //printf("i = %d, lower = %d\n", i, lower);
        if (searched[i]) continue;
        if ((A[i]->data) && (!(A[i]->enable))) {
          //printf("loop 4: found full cell @  index %d\n", i);
          searched[i] = true;
          repeat = true;
          max = i;
          upper = (upper < i + *clue) ? upper : i + *clue;
        }
      }
    } while (repeat);

    printf("u/l: %d-%d, m/m: %d-%d\n", upper, lower, min, max);

    //be careful of this part, could probs be error part
    //if last clue, just searches everywhere... wait
    //TODO: double check that this has the right cutoffs I don't trust it

    if (clues - clue_num == 1) {
      for (int i = f + *clue; i < d; i++) {
        if (A[i]->enable) continue;
        if (A[i]->data) max = ((max > i + 1) ? max : i + 1);
      }
      for (int i = d - 1; i >= f; i--) {
        if (A[i]->enable) continue;
        if (A[i]->data) min = ((min < i) ? min : i);
      }
    }

    printf("u/l: %d-%d, m/m: %d-%d\n", upper, lower, min, max);

    // TODO: if max == upper || max + 1 == empty
    //       if min == lower || min - 1 == empty

    if (max >= d)
      min = max - *clue;
    else if (max < upper - 1)
      if (!((A[max + 1]->data) || (A[max + 1]->enable))) min = max - *clue;

    if (min <= f)
      max = min + *clue;
    else if (min >= 1)
      if (!((A[min - 1]->data) || (A[min - 1]->enable)))
        max = min + *clue;

    // step 3.
    //printf("%hu: %d->%d\n", *clue, min, max);
    for (int i = min; i < max; i++)
      full_cells += fill_cell(A, i, d);

    // step 3a. add endcaps
    if (max - min == *clue) {
      empty_cells += empty_cell(A, min - 1, d);
      empty_cells += empty_cell(A, max, d);
    }

    // emptying out before
    if ((clues == 1) && (max - min == *clue_ptr))
      for (int i = f; i < min; i++)
        empty_cell(A, i, d);

    if (f > 0) {// cuts off any beyond the first clue
      f += *clue + 1;
      clue_num++;
      continue;
    }

    // could just go positive if min > max (which we don't want)
    // TODO: fix this MASSIVE bug that seems to be here. (I'm not sure if it's
    // still there btw)
    for (int i = f; i < lower; i++)
      empty_cells += empty_cell(A, i, d);

    /*
     * I have no idea what the hell this line does. It shouldn't really be here
    if (clues - clue_num == 1)
      for (int i = upper; i < d; i++)
        empty_cells += empty_cell(A, i, d);
    */

    // step 5
    clue_num++;
    if (max - min == *clue)
      f = max + 1;
    else
      f += *clue + 1;
  }

  // if all clues filled, make all cells empty
  // if all empty cells filled, make all cells full

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
    print_row(array[i], 0, width, blocks[0][i].arr, blocks[0][i].blocks);
    printf("\n");
  }

  printf("\nSTARTING COLUMNS\n\n");
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++)
      prob_arr[j] = array[j][i];
    curr_sum += solve_row(prob_arr, height, blocks[1][i].arr,
                          blocks[1][i].blocks);
    print_row(prob_arr, 0, height, blocks[1][i].arr, blocks[1][i].blocks);
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


