#include "io.h"

/*
 * called by get_block_data. It
 *
 *
 */
static unsigned short int WIDTH = 0;
static unsigned short int HEIGHT = 0;

static unsigned short int[WIDTH][HEIGHT] fetch_from_file(FILE *in_file,
                                              unsigned short int blocks[][]) {
  
  return blocks;
}

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
unsigned short int ***get_block_data(char *file_name) {

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
i
  if ((status < 2) || (height > MAX_SIZE) || (width > MAX_SIZE)) {
    printf("failed to read in width and height\n");
    fclose(in_file);
    return NULL;
  }

  printf("width: %d, height: %d\n", width, height);

  // mallocing row blocks ()
  unsigned short int (*blocks)[width][height] = malloc(sizeof(short[2][width]
                                                                   [height]));
  if (!blocks) {
    printf("malloc failed");
    fclose(in_file);
    return NULL;
  }

  printf("malloc good. (%lu)\n", sizeof(blocks));

  // reading in rows
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < height; j++) {
      int row_sum = -1;
      int num_blocks = 0;

      printf("1.\n");
      // finding the number of blocks in the row
      status = fscanf(in_file, "%d ", &num_blocks);
      if (status < 1) {
        fclose(in_file);
        free(blocks);
        return NULL;
      }

      printf("2.\n");
      printf("number: %d, ", num_blocks);
      fflush(NULL);

      // reading in blocks
      for (int k = 0; k < num_blocks; k++) {
        status = fscanf(in_file, "%hu ", &buffer);
        if ((status < 1)) {
          fclose(in_file);
          free(blocks);
          return NULL;
        }

        printf("3. ");
        fflush(NULL);

        printf("%hu", buffer);
        fflush(NULL);
        // saving value
        if (row_sum + buffer + 1 <= width) {
          printf("%hu (%d, %d, %d)", buffer, i, j, k);
          fflush(NULL);
          blocks[i][j][k] = buffer;
          row_sum += buffer + 1;
        }
      }

      fscanf(in_file, "\n");
      printf("\n");
    }
    fscanf(in_file, "\n");
    printf("\n");
  }

  return blocks;
}

int main(int argc, char *argv[]) {
  unsigned short int ***blocks = get_block_data("blocks.txt");
  if (!blocks)
    return -1;

  printf("%lu %lu", sizeof(blocks[0][0]) / sizeof(short int),
         sizeof(blocks[1][0]) / sizeof(short int));


  free(blocks);

}


