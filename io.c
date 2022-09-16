#include "io.h"

/*
 * called by get_block_data. It fetches the data from the file.
 *
 *
 */
static int fetch_from_file(FILE *in_file, int width, int height,
                           unsigned short int* blocks) {

  if ((!in_file) || (!blocks)) {
    printf("failed to find blocks\n");
    return -1;
  }

  int status;
  unsigned short int buffer;
  char buffer_buffer = 0;

  // reading in rows
  for (int i = 0; i < height; i++) {
    int row_sum = -1;

    // finding the number of blocks in the row

    printf("# %d, ", i);

    // reading in blocks
    for (int j = 0; j < width; j++) {
      status = fscanf(in_file, " %hu", &buffer);
      printf("%hu", buffer);
      if ((status < 1)) {
        return -1;
      }

      status = fscanf(in_file, "%c", &buffer_buffer);

      fflush(NULL);
      // saving value
      if (row_sum + buffer + 1 <= width) {
        fflush(NULL);
        *(blocks + i * width + j) = buffer;
        row_sum += buffer + 1;
      }
      if (buffer_buffer != ' ') {
        break;
      }
    }
    if (buffer_buffer != '\n')
      status = fscanf(in_file, "\n");
    printf("\n");
  }
  if (buffer_buffer != '\n')
    status = fscanf(in_file, "\n");
  printf("\n");

  return 1;
}

/*
 * returns 1 if it works, returns an error code if it fails.
 *
 * Reads in a set of blocks for a nonogram from a file. The input sequence is as
 * follows: (brackets just mean it's in place of a number, ... means repeat the
 * pattern as nessesary)
 *
 * [width] [height]
 * [block 1 length] [block 2 length]
 * [block 1 length] ...
 * ...
 * [row n # of blocks] ...
 *
 * [block 1 length]...
 *
 * note, the total length (and spaces between each block) can't be longer than
 * the length of that row / length.
 *
 */
unsigned short int *get_block_data(char *file_name, int *width_ptr,
                                   int *height_ptr) {

  if ((!file_name))
    return NULL;

  FILE *in_file;

  //initalizing FILE
  in_file = fopen(file_name, "r");

  if (!in_file)
    return NULL;

  int status;

  int height, width;

  //getting dimensions of nonogram
  status = fscanf(in_file, "%d %d\n\n", &height, &width);

  if (status < 2) {
    printf("failed to read in width and height\n");
    fclose(in_file);
    return NULL;
  }

  //printf("width: %d, height: %d\n", width, height);

  *height_ptr = height;
  *width_ptr = width;

  if ((width > MAX_SIZE) || (width <= 0) ||
      (height > MAX_SIZE) || (height <= 0)) {
    fclose(in_file);
    in_file = NULL;
    return NULL;
  }

  unsigned short int *blocks =
                      malloc(sizeof(unsigned short int) * width * height * 2);
  if (!blocks) {
    return NULL;
  }

  status = fetch_from_file(in_file, height, width, blocks);
  if (status < 1) {
    free(blocks);
    blocks = NULL;
    fclose(in_file);
    in_file = NULL;
    return NULL;
  }

  status = fetch_from_file(in_file, width, height, blocks + width * height);
  if (status < 1) {
    free(blocks);
    blocks = NULL;
    fclose(in_file);
    in_file = NULL;
    return NULL;
  }

  return blocks;
}

int main(int argc, char *argv[]) {
  int height, width;
  unsigned short int *blocks = get_block_data("blocks.txt",
                                              &height, &width);
  if (!blocks)
    return -1;

  printf("Function good.\n");
  printf("width = %d, height = %d\n", width, height);

  for (int j = 0; j < height; j++) {
    for (int k = 0; k < width; k++) {
      int offset = j * width + k;
      if ((*(blocks + offset) == 0) && (k != 0))
        break;
      printf("%hu ", *(blocks + offset));
    }
    printf("\n");
  }
  printf("\n");

  for (int j = 0; j < width; j++) {
    for (int k = 0; k < height; k++) {
      int offset = width * height + j * height + k;
      if ((*(blocks + offset) == 0) && (k != 0))
        break;
      printf("%hu ", *(blocks + offset));
    }
    printf("\n");
  }
  printf("\n");


  free(blocks);
}
