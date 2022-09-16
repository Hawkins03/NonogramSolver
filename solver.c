#include "solver.h"

//reconfigure, not reseting prob_arr[i] to -1 or -2, instead, empty = -1,
//full = 1 and undecided = 0. This way, I should have to do less work to mess
//with things. IDK why i decided to mess up the pointers as well.
//

void check_pointers(int width, int height, cell_t *grid[height][width]) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      printf("%d, %d: %p\n", i, j, grid[i][j]);
      fflush(NULL);
    }
  }
}

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
void note_branch(cell_t **prob_arr, int front_offset, int end_offset, int size,
                 unsigned short int *block_arr, int blocks, int branch_num) {
  for (int i = 0; i < blocks; i++)
    printf("%hu ", block_arr[i]);

  printf("|");
  for (int i = 0; i < front_offset; i++) {
    printf("-");
  }
  for (int i = 0; i < size; i++) {
    if (!(prob_arr[i]->enable)) {
      if (prob_arr[i]->data)
        printf("%c", 219);
      else
        printf("X");
    }
    else
      printf("?");
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
      if (!grid[i][j]->enable) {
        if (grid[i][j]->data)
          printf("X");
        else
          printf(" ");
      }
      else {
        printf("?");
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
  int num_found = 0;
  if ((!blocks) || (!(*block_arr))) {
    for (int i = 0; i < length; i++) {
      (prob_arr[i])->data = 0;
      num_found+= (prob_arr[i])->enable;
      (prob_arr[i])->enable = 0;
    }
    return num_found;
  }

  // counting the length
  int total_block_len = -1;
  for (int i = 0; i < blocks; i++)
    total_block_len += block_arr[i] + 1;

  if (length < total_block_len)
    return num_found;


  printf("block: %hu ttl: %d len: %d ttl_len %d\n", *block_arr, blocks, length,
         total_block_len);

  if (*block_arr > length - total_block_len) {
    printf("filling blocks:\n");
    printf(" looking for constricting factors\n");
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
        note_branch(prob_arr + *block_arr + 1, *block_arr + 1, 0,
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
        num_found += solve(prob_arr + i + 1, length - i - 1, block_arr, blocks);
        return num_found;
      }
    }

    // 6 || | | | |START HERE||END HERE| | | | ||
    // Filling in the parts that have to be full
    printf("Filling middle\n");
    for (int i = length - total_block_len; i < block_arr[0]; i++) {
      printf("i: %d", i);
      prob_arr[i]->data = 1;
      num_found+=prob_arr[i]->enable;
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
  int offset;
  if (total_block_len > (length - block_arr[0]))
    offset = total_block_len;
  else
    offset = length - block_arr[0];

  for (int i = offset; i < length; i++) {
    if ((blocks == 1) && (prob_arr[i]->enable)) {
      if (prob_arr[i]->data)
        solve(prob_arr + length - i - block_arr[0],
              i + *block_arr, block_arr, 1);
      else {
        for (int j = i; j < length; j++) {
          prob_arr[j]->data = 0;
          num_found += prob_arr[j]->enable;
          prob_arr[j]->enable = 0;
        }
        solve(prob_arr, length - i, block_arr, blocks);
      }
    }
  }

  num_found += solve(prob_arr + block_arr[0] + 2, length - block_arr[0] - 2,
                     block_arr + 1, blocks - 1);
  return num_found;
}

int solver_loop(int width, int height, cell_t *array[height][width],
                unsigned short int *blocks) {
  cell_t *prob_arr[width];
  print_grid(width, height, array);
  //check_pointers(width, height, array);
  int curr_sum = 0;

  //solving rows
  for (int i = 0; i < height; i++) {
    curr_sum += solve(array[i], width, blocks + height * i, width / 2);
  }

  //solving columns
  for (int i = 0; i < height; i++) {
    for (int j = i; j < width; j++)
      prob_arr[j] = array[i][j];

    curr_sum += solve(prob_arr, height, blocks + width * height + width * i,
                      height / 2);
  }

  // counting up total and comparing to prev loop to see if we can run again
  // or if we've stagnated.

  return curr_sum;
}

int main(int argc, char *argv[]) {
  int width, height;

  char *file_name = "blocks.txt";

  unsigned short int *blocks = get_block_data(file_name, &height, &width);
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
      printf("%d, %d: %p %p\n", i, j, &array_setup[i][j], array[i][j]);
    }
  }

  printf("setup done!\n");

  while (1) {
    int curr_sum = solver_loop(width, height, array, blocks);
    print_grid(width ,height, array);
    if (prev_sum >= curr_sum) {
      break;
    }
  }

  return 0;
}
