#include "io.h"

/*
 * returns 1 if it works, returns an error code if it fails.
 *
 * output acts as a punch card, make it, then plug it into the function to get
 * your result. (I don't really want to mess with returing a pointer to
 * something that no longer exists after the function ends). I used shorts
 * instead of ints because I didn't need int lengths, and 2 bytes is honestly a
 * bit much, I could deff get away with only 1, but just for size reasons I'm
 * leaving it at a short for now. (I may edit this later)
 *
 * Steps required:
 *  1. open the file
 *  2. read the things inside the file
 *   3. Conveniently find a way to use the regex grouping to auto seperate the
 *   info where it needs to go. (group 1 = rows, 4 = columns, 2 & 5 are
 *   individual rows and columns, 3 & 6 are numbers)
 *
 * regex stuff to be added later:
 *  regex_t regex;
 *  char *filter = "(((\\d+) ?){0,15}\n?){0,30}\n(((\\d+) ?){0,15}\n?){0,30}";

 */
int get_block_data(char *file_name,
                   unsigned short int *rows[MAX_SIZE][MAX_SIZE / 2],
                   unsigned short int *columns[MAX_SIZE][MAX_SIZE / 2]) {
  if ((!file_name) || (!rows) || (!columns))
    return NULL_INPUT;
  int status;
  char buffer[31] = { 0 };
  FILE *in_file;

  /* get in the file data 1860 (30 * 31 * 2 + 1 - 1) buffer size, or do it
   * using for loops */

  in_file = fopen(file_name, "r");
  if (!in_file)
    return FILE_ERROR;

  // reading in rows
  for (int i = 0; i < 30; i++) {
    int row_sum = -1;
    status = fscanf(in_file, "%[^\n]", buffer);
    // parse through it using regex because scanf ignores whitespace (which
    // means it will overflow the buffer. Solution: Use regex to parse through
    // it. This way it's also more space effecient, 31 bytes instead of 1860 at
    // a time.)

    if ((status < 1))
      return FILE_ERROR;

    // don't read in all the stupid enters (" " = all whitespace as does \n)
    if (status == 0)
      break;

    if (row_sum > MAX_SIZE)
      return INPUT_ERROR;
    status = fscanf(in_file, "\n");
  }

  //regex: ";

  return 1;
}

