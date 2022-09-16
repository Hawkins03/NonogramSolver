#include "solver.h"

/*
 * prob_arr = probability array from solve
 * front_offset = how much you plan to shorten it from the front
 * end_offset = how much you plan to shorten it from the end
 * size = the new size of the prob arr
 * block_arr = the array of blocks from solve
 * blocks = the number of blocks
 * branch_num = the branch location
 *
 * prints the debug for the branch.
 */
static void note_branch(cell_t **prob_arr, int front_offset, int end_offset,
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
}

/*
 * prints the new grid for each itteration.
 */
void print_grid(int width, int height, cell_t *grid[height][width]) {
  if (!grid)
    printf("Error, null grid.");
  if ((width <= 0) || (height <= 0) || (width > MAX_SIZE) || (height > MAX_SIZE))
    printf("error, dimensions out of bounds.");

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (grid[i][j]->enable) {
        printf("?");
      }
      else {
        if (grid[i][j]->data)
          printf("X");
        else
          printf(".");
      }
    }
    printf("\n");
    fflush(NULL);
  }
}

/*
 * solves a row / column in a nonogram.
 * returns the sum of the empty / full blocks found.
 *
 * Takes in a probability array (pointers to the main array)
 * also takes in the length of that array.
 *    (makes it easier to constrain the length when recursing)
 * In addition, it takes in an array of blocks (the little numbers to the side/
 * top of the nonogram)
 * also takes in the length of that array (again to make it easier to constrain)
 * quick flowchart:
 * Start:
 *  if length <= 0 || blocks <= 0 -> end
 *  Find total block len

 *  is the current block long enough to fill spaces?
 *   check before the spaces to be filled to see if we can constrain the length
 *   fill the spaces
 *
 *  if not, just check for constraints on the length
 * end.
 */
int solve(cell_t **prob_arr, int length, unsigned short int *block_arr,
                                                                  int blocks) {
  /* checking for blocks being done */
  int num_found = 0;
  if ((blocks == 0) || (*block_arr == 0)) {
    for (int i = 0; i < length; i++) {
      (prob_arr[i])->data = 0;
      num_found+= (prob_arr[i])->enable;
      (prob_arr[i])->enable = 0;
    }
    return num_found;
  }

  // counting the length
  int total_block_len = blocks - 1;
  for (int i = 0; i < blocks; i++)
    total_block_len += block_arr[i];

  if (length < total_block_len) {
    return num_found;
  }

  if (*block_arr > length - total_block_len) {
    // 6 || | | |END HERE| || | | | | ||
    // making sure there's nothing to change length
    for (int i = 0; i < length - total_block_len; i++) {
      if (prob_arr[i]->enable) // ignores any undecided blocks
        continue;

      if (prob_arr[i]->data) {
        // ---------------------------- BRANCH 1 ------------------------------
        note_branch(prob_arr, 0, *block_arr + i, length - *block_arr + i,
                    block_arr, 1, 1);
        num_found += solve(prob_arr, length - *block_arr + i, block_arr, 1);
        // ---------------------------- BRANCH 2 ------------------------------
        note_branch(prob_arr + *block_arr + 2, *block_arr + 2, 0,
                    length - *block_arr - 1, block_arr + 1, blocks - 1, 2);
        num_found += solve(prob_arr + block_arr[0] + 1, length - block_arr[0]
                           - 1, block_arr + 1, blocks - 1);
        return num_found;
      }
      else { // empty
        for (int j = 0; j < i; j++) {
          prob_arr[j]->data = 0;
          num_found += prob_arr[j]->enable;
          prob_arr[j]->enable = 0;
        }
        // constricting to beyond i.
        // ---------------------------- BRANCH 3 ------------------------------
        note_branch(prob_arr, i + 1, 0, length - i - 1, block_arr, blocks, 3);
        num_found += solve(prob_arr + i + 2, length - i - 2, block_arr, blocks);
        return num_found;
      }
    }

    // 6 || | | | |START HERE||END HERE| | | | ||
    // Filling in the parts that have to be full
    for (int i = length - total_block_len; i < block_arr[0]; i++) {
      prob_arr[i]->data = 1;
      num_found += prob_arr[i]->enable;
      prob_arr[i]->enable = 0;
    }
  }
  else {
    // 3||START HERE| |END HERE| | || | ||
    for (int i = 0; i < block_arr[0]; i++) {
      if (prob_arr[i]->enable) // skips any undecided
        continue;

      if (prob_arr[i]->data) { // other forward part, same as b4.
        // ---------------------------- BLOCK 4 -------------------------------
        note_branch(prob_arr, 0, length - *block_arr - i, *block_arr + i,
                    block_arr, 1, 4);
        num_found += solve(prob_arr, block_arr[0] + i, block_arr, 1);
        // ---------------------------- BLOCK 5 -------------------------------
        note_branch(prob_arr, *block_arr + 2, 0, length - *block_arr - 2,
                    block_arr + 1, blocks - 1, 5);
        num_found += solve(prob_arr + block_arr[0] + 2, length - block_arr[0]
                           - 2, block_arr + 1, blocks - 1);
        return num_found;
      }
      else {
        // ---------------------------- BLOCK 6 -------------------------------
        note_branch(prob_arr + i + 2, i + 2, 0, length - i - 2, block_arr,
            blocks, 6);
        num_found += solve(prob_arr + i + 2, length - i - 2, block_arr,
                            blocks);
         return num_found;
      }
    }
  }

  // 6 || | | | | || |START HERE| | |END HERE||
  // 3 || | | |START HERE| || |END HERE||
  // Fnding constraints to the length
  int offset;
  if (total_block_len > (length - block_arr[0]))
    offset = total_block_len;
  else
    offset = length - block_arr[0];

  for (int i = offset; i < length; i++) {
    if ((prob_arr[i]->enable) || (blocks > 1))
      continue;

    if (prob_arr[i]->data) {
      // ---------------------------- BLOCK 7 -------------------------------
      note_branch(prob_arr + length - i - *block_arr, length - i - *block_arr,
                  0, i + *block_arr, block_arr, 1, 7);
      solve(prob_arr + length - i - *block_arr,
            i + *block_arr, block_arr, 1);
    }
    else {
      for (int j = i; j < length; j++) {
        prob_arr[j]->data = 0;
        num_found += prob_arr[j]->enable;
        prob_arr[j]->enable = 0;
      }
      // ---------------------------- BLOCK 8 -------------------------------
      note_branch(prob_arr, 0, i, length - i, block_arr, blocks, 8);
      solve(prob_arr, length - i, block_arr, blocks);
    }
  }

  if (blocks == 1)
    return num_found;
  // ---------------------------------- BLOCK 9 -------------------------------
  note_branch(prob_arr + *block_arr + 1, *block_arr, 0,
              length - *block_arr - 2, block_arr + 1, blocks - 1, 9);
  fflush(NULL);
  num_found += solve(prob_arr + *block_arr + 1, length - *block_arr - 1,
                     block_arr , blocks - 1);
  return num_found;
}

int solver_loop(int width, int height, cell_t *array[height][width],
                block_arr_t **blocks) {
  cell_t *prob_arr[width];
  int curr_sum = 0;

  printf("\nSTARTING ROWS\n\n");
  //solving rows
  for (int i = 0; i < height; i++) {
    curr_sum += solve(array[i], width, blocks[0][i].arr, blocks[0][i].blocks);
    note_branch(array[i], 0, 0, width, blocks[0][i].arr, blocks[0][i].blocks,
                0);
  }


  printf("\nSTARTING COLUMNS\n\n");
  //solving columns
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++)
      prob_arr[j] = array[i][j];
    curr_sum += solve(prob_arr, height, blocks[1][i].arr, blocks[1][i].blocks);
    note_branch(array[i], 0, 0, height, blocks[1][i].arr, blocks[1][i].blocks,
                0);

  }

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
  int prev_sum = 0;
  cell_t array_setup[width][height]; // make setup_array thingey.
  cell_t *array[width][height];

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      array[i][j] = &array_setup[i][j];
      array_setup[i][j].data = 0;
      array_setup[i][j].enable = 1;
    }
  }

  printf("setup done!\n");

  while (1) {
    int curr_sum = solver_loop(width, height, array, blocks);
    if (prev_sum >= curr_sum) {
      break;
    }
    prev_sum = curr_sum;
  }

  return 0;
}
