#include "solver.h"

//reconfigure, not reseting prob_arr[i] to -1 or -2, instead, empty = -1,
//full = 1 and undecided = 0. This way, I should have to do less work to mess
//with things. IDK why i decided to mess up the pointers as well.

int solve(int *prob_arr[], int length, int block_arr[], int blocks) {
  //base cases
  if (blocks == 0) {
    for (int i = 0; i < length; i++)
      mark_empty(&prob_arr[i]);
    return 0;
  }
  if (length <= 0)
    return 0;

  // counting the length
  int total_block_len = -1;
  for (int i = 0; i < blocks; i++)
    total_block_len += block_arr[i] + 1;


  if (block_arr[0] > length - total_block_len) {
    // 6 || | | |END HERE| || | | | | ||
    // making sure there's nothing to change length
    for (int i = 0; i < length - total_block_len; i++) {
      if (*prob_arr[i] == FULL) {
        solve(prob_arr, length - block_arr[i] + i, block_arr, 1);
        solve(prob_arr + block_arr[0] + 1, length - block_arr[0] - 1,
              block_arr + 1, blocks - 1);
        return 1;
      }
      else if (*prob_arr[i] == EMPTY) {
        solve(prob_arr + i + 1, length - i - 1, block_arr, blocks);
        return 1;
      }
    }

    // 6 || | | | |START HERE||END HERE| | | | ||
    // Filling in the parts that have to be full
    for (int i = length - total_block_len; i < total_block_len; i++) {
      if ((*prob_arr[i] == EMPTY) && (i < block_arr[0])) {
        for (int j = 0; j < i; j++)
          *prob_arr[i] = EMPTY;
        solve(prob_arr + i + 1, length - i - 1, block_arr, blocks);
        return 1;
      }
      else if (!prob_arr[i]) // 0 = undecided (this should work as i intend it)
        *prob_arr[i] = FULL;
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
      if (*prob_arr[i] == FULL) {
        solve(prob_arr, block_arr[0] + i, block_arr, 1);
        return solve(prob_arr + block_arr[0] + 1, length - block_arr[0] - 1,
                     block_arr + 1, length - 1);
      }
      if (*prob_arr[i] == EMPTY) {
        return solve(prob_arr + i + 1, length - i - 1, block_arr, blocks);
      }
    }
  }

  // 6 || | | | | || |START HERE| | |END HERE||
  // 3 || | | |START HERE| || |END HERE||
  // Fnding constraints to the length
  for (int i = total_block_len; i < length; i++) {
    if (*prob_arr[i] == EMPTY) {
      if (blocks == 1) {
        for (int j = i; j < length; j++)
          if (!(*prob_arr[j]))
            *prob_arr[j] = EMPTY;
        return 1;
      }

      solve(prob_arr, i, block_arr, blocks);
    }
  }

  solve(prob_arr + block_arr[0] + 1, length - block_arr[0] - 1, block_arr + 1,
        blocks + 1);
  return 1;
}


int main(int argc, char *argv[]) {
  // TEMP VARIABLES UNTIL I CAN MAKE A REGEX TO PULL INFO IN
  int width = 10;
  int height = 10;

  //non-temp variables
  int prev_sum = 0;
  int array[width][height]; // make setup_array thingey.
  int row_blocks[width][height / 2];
  int column_blocks[width][height / 2];
  int *prob_arr[] = { 0 };

  while (1) {
    //solving rows
    for (int i = 0; i < width; i++) {
      for (int j = i; j < height; j++)
        prob_arr[j] = &array[i][j];

      solve(prob_arr, width, row_blocks[i],
            (int) sizeof(row_blocks[i]) / sizeof(int));
    }

    //solving columns
    for (int i = 0; i < height; i++) {
      for (int j = i; j < width; j++)
        prob_arr[j] = &array[i][j];

    solve(prob_arr, width, column_blocks[i],
          (int) sizeof(column_blocks[i]) / sizeof(int));
    }

    // counting up total and comparing to prev loop to see if we can run again
    // or if we've stagnated.
    int total_found = 0;
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        if (array[i][j] < 0)
          total_found++;
      }
    }

    if (total_found <= prev_sum) {
      //display array
      return 0;
    }

    prev_sum = total_found;
  }
  return 0;
}
