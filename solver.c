#include "solver.h"

//reconfigure, not reseting prob_arr[i] to -1 or -2, instead, empty = -1,
//full = 1 and undecided = 0. This way, I should have to do less work to mess
//with things. IDK why i decided to mess up the pointers as well.
//

void print_grid(cell_t *grid[MAX_SIZE][MAX_SIZE], int width, int height) {
  if (!grid)
    printf("Error, null grid.");
  if ((width <= 0) || (height <= 0) || (width > MAX_SIZE) || (height > MAX_SIZE))
    printf("error, dimensions out of bounds.");

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      switch ((short) grid[i][j]->enable + (short) 2* grid[i][j]->data) {
        case 3:
          printf("X");
          break;
        case 1:
          printf(" ");
          break;
        default:
          printf("?");
          break;
      }
    }
    printf("\n");
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
int solve(cell_t *prob_arr[], int length, int block_arr[], int blocks) {
  int num_found = 0;
  if (blocks == 0) {
    for (int i = 0; i < length; i++) {
      prob_arr[i]->data = 0;
      num_found+= prob_arr[i]->enable;
      prob_arr[i]->enable = 0;
    }
    return num_found;
  }

  if (length <= 0)
    return num_found;

  // counting the length
  int total_block_len = -1;
  for (int i = 0; i < blocks; i++)
    total_block_len += block_arr[i] + 1;


  if (block_arr[0] > length - total_block_len) {
    // 6 || | | |END HERE| || | | | | ||
    // making sure there's nothing to change length
    for (int i = 0; i < length - total_block_len; i++) {
      if (prob_arr[i]->enable)
        break;

      if (prob_arr[i]->data) {
        num_found += solve(prob_arr, length - block_arr[i] + i, block_arr, 1);
        num_found += solve(prob_arr + block_arr[0] + 1, length - block_arr[0]
                           - 1, block_arr + 1, blocks - 1);
        return num_found;
      }
      else {
        num_found += solve(prob_arr + i + 1, length - i - 1, block_arr, blocks);
        return num_found;
      }
    }

    // 6 || | | | |START HERE||END HERE| | | | ||
    // Filling in the parts that have to be full
    for (int i = length - total_block_len; i < block_arr[0]; i++) {
      for (int j = 0; j < i; j++) {
        prob_arr[i]->data = 1;
        num_found+=prob_arr[i]->enable;
        prob_arr[i]->enable = 0;
      }

      num_found += solve(prob_arr + i + 1, length - i - 1, block_arr, blocks);
      return num_found;
    }
  }

  /*
   * --------------------PROGRAMMER NOTES---------------------------
   * line 42 (if ((*prob...)
   * was built to handle the following situation:
   * 6|| | | | |0|| | | | | || || *0 = empty, X = FULL " "= undecided
   * If I found a simialar situation (I won't now) where the loop started before
   * that 0, then the array would look like this afterwords:
   * 6|| | |X|X|0|| | | | | || ||
   * impossible instead of:
   * 6||0|0|0|0|0|| | | | | || ||.
   * (as it stands, the loop would start at that 0 and then set the earlier
   * numbers to 0, (then move on to solve the damn thing.))
   */

  else {
    // 3||START HERE| |END HERE| | || | ||
    for (int i = 0; i < block_arr[0]; i++) {
      if (prob_arr[i]->enable)
        break;

      if (prob_arr[i]->data) {
        num_found += solve(prob_arr, block_arr[0] + i, block_arr, 1);
        num_found += solve(prob_arr + block_arr[0] + 1, length - block_arr[0]
                           - 1, block_arr + 1, length - 1);
        return num_found;
      }
      else {
         num_found += solve(prob_arr + i + 1, length - i - 1, block_arr,
                            blocks);
         return num_found;
      }
    }
  }

  // 6 || | | | | || |START HERE| | |END HERE||
  // 3 || | | |START HERE| || |END HERE||
  // Fnding constraints to the length
  for (int i = total_block_len; i < length; i++) {
    if (prob_arr[i]->enable && prob_arr[i]->data) {// full
      if ((length - i - blocks[0] >= 0) && (length - blocks[0] < i))
        solve(prob_arr + length - i - blocks[i])
    }
    else if (prob_arr[i]->enable) {

    }

    /* I'm confused, and I know I wrote this wrong. I'm probably going to
     * revert it a bit and start from there
    if ((prob_arr[i]->data) * (1 - prob_arr[i]->enable)) { // known full
      if (blocks == 1) {
        num_found += solve(prob_arr, i, block_arr, 1);
        return num_found;
      }

      num_found += solve(prob_arr, i - 1, block_arr, blocks);
    }

    if ((1 - prob_arr[i]->data) * (1 - prob_arr[i]->enable)) { //known empty
      int offset = length - block_arr[0] - i;
      if (offset >= 0) {
        num_found += solve(prob_arr + offset, length - offset,
                           block_arr, blocks);

        if (blocks == 1) {
          for (int j = 0; j < offset; j++) {
            prob_arr[j]->data = 0;
            num_found += prob_arr[i]->enable;
            prob_arr[i]->enable = 0;
          }
        }
      }
      else
        return -1; //WEE WOO WEE WOO, PROBLEM PROBLEM.
    }*/
  }

  num_found += solve(prob_arr + block_arr[0] + 1, length - block_arr[0] - 1,
                     block_arr + 1, blocks + 1);
  return num_found;
}


int main(int argc, char *argv[]) {
  int width, height;

  char *file_name = "blocks.txt";
  FILE *in_file = fopen(file_name, "r");

  if (!in_file) {
    printf("error opening file.");
    return -1;
  }

  unsigned short int *blocks = get_block_data(file_name, &height, &width);


  //non-temp variables
  int prev_sum = 0;
  cell_t *array[width][height]; // make setup_array thingey.
  int row_blocks[width][height / 2] = ;
  int column_blocks[width][height / 2];
  cell_t *prob_arr[] = { 0 };

  while (1) {
    int curr_sum = 0;

    //solving rows
    for (int i = 0; i < width; i++) {
      for (int j = i; j < height; j++)
        prob_arr[j] = array[i][j];

      curr_sum += solve(prob_arr, width, row_blocks[i],
                        (int) sizeof(row_blocks[i]) / sizeof(int));
    }

    //solving columns
    for (int i = 0; i < height; i++) {
      for (int j = i; j < width; j++)
        prob_arr[j] = array[i][j];

    curr_sum += solve(prob_arr, width, column_blocks[i],
                      (int) sizeof(column_blocks[i]) / sizeof(int));
    }

    // counting up total and comparing to prev loop to see if we can run again
    // or if we've stagnated.

    if (curr_sum <= prev_sum) {
      //display array
      return 0;
    }

    prev_sum = curr_sum;
  }
  return 0;
}
