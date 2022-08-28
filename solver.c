#include "solver.h"

int mark_empty(int **index_ptr) {
  if (!(index_ptr) || !(*index_ptr))
    return -1;

  *(*(index_ptr)) = EMPTY;
  *index_ptr = (int *) EMPTY;

  return 1;
}

int mark_full(int **index_ptr) {
  if (!(index_ptr) || (!(*index_ptr)))
    return -1;
  **index_ptr = FULL;
  *index_ptr = (int *) FULL;

  return 1;
}

void check_status(int *prob_arr[], int length, int block_arr[], int blocks) {
  int total_block_len = -1;
  for (int i = 0; i < blocks; i++)
    total_block_len += block_arr[i] + 1;

  int total_full = 0;
  int total_empty = 0;
  for (int i = 0; i < length; i++) {
    if (prob_arr[i] == (int *) EMPTY)
      total_empty++;
    else if (prob_arr[i] == (int *) FULL)
      total_full++;
    else {
      if (*prob_arr[i] == 0) {
        *(prob_arr[i]) = EMPTY;
        prob_arr[i] = (int *) EMPTY;
        total_empty++;
      }
      else if (*prob_arr[i] <= length - total_block_len) {
        *prob_arr[i] = FULL;
        prob_arr[i] = (int *)FULL;
        total_full++;
      }
    }
  }
  //sum_blocks if total_empty == length - sum_blocks  -> rest = full
  //if total_full == sum_blocks -> rest = empty
}

/*
//check if 1 - # empty = sum(block_arr)
int check_full(int *prob_arr[], int length, int block_arr[], int blocks) {
  int num_empty = 0;
  for (int i = 0; i < length; i++)
    if (prob_arr[i] > 0) {
      if ((*prob_arr[i]) >= length - total_block_len) {
        mark_full(&prob_arr[i]);
      }
    }
    else if ((prob_arr[i] == (int *) EMPTY) && (prob_arr[i] != (int *) FULL))
      num_empty++;

  return 1;
}*/

int solve(int *prob_arr[], int length, int block_arr[], int blocks) {
  if (blocks == 0) {
    for (int i = 0; i < length; i++)
      mark_empty(&prob_arr[i]);
    return 0;
  }

  if (length == 0)
    return 0;

  int total_block_len = -1;
  for (int i = 0; i < blocks; i++)
    total_block_len += block_arr[i] + 1;

  for (int i = 0; i < length - total_block_len; i++) {
    if (prob_arr[i] < 0)
      continue;
    for (int j = i + 1; j < block_arr[0]; j++)
      if (prob_arr[j] > 0)
        (*prob_arr[j])++;

    if (prob_arr[i] == (int *) EMPTY) {
      solve(prob_arr + i, length - i, block_arr, blocks);
      return 1;
    }
    if (prob_arr[i] == (int *) FULL) {
      solve(prob_arr, i + block_arr[i], &block_arr[i], 1);
      solve(prob_arr + block_arr[0] + 1, length - block_arr[0] - 1,
            block_arr + 1, blocks - 1);
      return 1;
    }
  }
  solve(prob_arr + block_arr[0] + 1, length - block_arr[0] - 1, block_arr + 1,
        blocks + 1);
  return 1;
}

int main(int argc, char *argv[]) {
  int width = 10;
  int height = 10;
  int prev_sum = 0;
  int array[width][height]; // make setup_array thingey.
  int row_blocks[width][height / 2];
  int column_blocks[width][height / 2];
  int *prob_arr[] = { 0 };

  while (1) {
    for (int i = 0; i < width; i++) {
      for (int j = i; j < height; j++) {
        switch (array[i][j]) {
          case EMPTY:
            prob_arr[j] = (int *) EMPTY;
            break;
          case FULL:
            prob_arr[j] = (int *) FULL;
            break;
          default:
            prob_arr[j] = &array[i][j];
            break;
        }
      }
 
      solve(prob_arr, width, row_blocks[i],
            (int) sizeof(row_blocks[i]) / sizeof(int));
      check_full(prob_arr, width, row_blocks[i],
                 (int) sizeof(row_blocks[i]) / sizeof(int));
      //check_empty;
    }

    for (int i = 0; i < height; i++) {
      for (int j = i; j < width; j++) {
        switch (array[i][j]) {
          case EMPTY:
            prob_arr[j] = (int *) EMPTY;
            break;
          case FULL:
            prob_arr[j] = (int *) FULL;
            break;
          default:
            prob_arr[j] = &array[i][j];
            break;
        }
      }

    solve(prob_arr, width, column_blocks[i], 
          (int) sizeof(column_blocks[i]) / sizeof(int));
    check_full(prob_arr, width, column_blocks[i],
               (int) sizeof(column_blocks[i]) / sizeof(int));
    //check_empty;
    }

    int sum_blocks = 0; // calculate
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height / 2; j++) {
        if (row_blocks[i][j] <= 0)
          break;
        sum_blocks++;
      }
    }

    for (int i =0; i < height; i++) {
      for (int j = 0; j < width / 2; j++) {
        if (column_blocks[i][j] <= 0)
          break;
        sum_blocks++;
      }
    }
    int total_full = 0;
    int total_empty = 0;
    int sum_probabilities = 0;
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        switch (array[i][j]) {
          case EMPTY:
            total_empty++;
            break;
          case FULL:
            total_full++;
            break;
          default:
            sum_probabilities += array[i][j];
        }
      }
    }

    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        if (array[i][j] >= 0)
          sum_probabilities += i;
        else
          sum_probabilities += width;
      }
    }
    if (sum_probabilities <= prev_sum) {
      //display array
      return 0;
    }

    prev_sum = sum_probabilities;
  }
  return 0;
}
