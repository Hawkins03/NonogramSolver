#include "io.h"

/*
 * returns 1 if it works, returns an error code if it fails.
 *
 * Steps required:
 *  1. open the file
 *  2. read the number of rows
 *   3. read in that many lines into a buffer each (and then use regex to
 *        convert each buffer into a list of integers)
 *  4. read in the number of columns.
 *   5. read in that many lines into a buffer each (and then use regex to
 *        convert each buffer into a list of integers)
 *
 * regex stuff to be added later:
 * char *filter = "\\(\\d+\\)"; // per line
 * 1860
 * returns a list containing pointers to both arrays
 */
int ***get_block_data(char *file_name) {

  if ((!file_name))
    return NULL;

  FILE *in_file;

  //initalizing FILE
  in_file = fopen(file_name, "r");

  if (!in_file)
    return NULL;

  int status;
  unsigned short int buffer;
  unsigned short int height;
  unsigned short int width;


  //getting dimensions of nonogram
  status = fscanf(in_file, "%hu %hu\n", &height, &width);

  if ((status < 2) || (height > MAX_SIZE) || (width > MAX_SIZE)) {
    fclose(in_file);
    return NULL;
  }

  // mallocing row blocks ()
  unsigned short int ***blocks = malloc(height * width *
                                           sizeof(short));
  if (!blocks) {
    fclose(in_file);
    return NULL;
  }

  // reading in rows
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < height; j++) {
      int row_sum = -1;
      int num_blocks = 0;

      // finding the number of blocks in the row
      status = fscanf(in_file, "%d ", &num_blocks);
      if (status < 1) {
        fclose(in_file);
        free(blocks);
        return NULL;
      }

      // reading in blocks
      for (int k = 0; k < num_blocks; k++) {
        status = fscanf(in_file, "%hu ", &buffer);
        if ((status < 1)) {
          fclose(in_file);
          free(blocks);
          return NULL;
        }

        // saving value
        if (row_sum + buffer + 1 <= width) {
          blocks[i][j][k] = buffer;
          row_sum += buffer + 1;
        }
      }

      fscanf(in_file, "\n");
    }
    fscanf(in_file, "\n");
  }

  return blocks;
}

