#include "io.h"

/*
 * a way for me to close everything if something doesn't work (this would be
 * a pain to copy-paste 6 times - 8 times)
 */
void close_vars(FILE *in_file, block_arr_t *rows, block_arr_t *cols,
                       block_arr_t **blocks, int height, int width) {
  if (in_file) {
    fclose(in_file);
    in_file = NULL;
  }
  if (rows) {
    for (int i = 0; i < height; i++) {
      if (rows[i].arr) {
        free(rows[i].arr);
        rows[i].arr = NULL;
      }
    }
    free(rows);
    rows = NULL;
  }
  if (cols) {
    for (int i = 0; i < width; i++) {
      if (cols[i].arr) {
        free(cols[i].arr);
        cols[i].arr = NULL;
      }
    }
    free(cols);
    cols = NULL;
  }
  if (blocks) {
    free(blocks);
    blocks = NULL;
  }
}

/*
 * called by get_block_data. It fetches the data from the file. It asks for the
 * file pointer and not the name, because the height and width are put in the
 * file. (I will probs choose to have them be inputted in the terminal line
 * along with the file name later).
 * in_file = file pointer
 * lines = number of lines to be read in
 * max_len = the max number of 
 */
static int fetch_from_file(FILE *in_file, int max_blocks,
                           block_arr_t row) {
  int status;
  unsigned short int buffer;
  char buffer_buffer = 0;

  // reading in blocks
  for (int j = 0; j < row.blocks; j++) {
    status = fscanf(in_file, " %hu", &buffer);
    printf("%hu", buffer);
    if (status < 1)
      return -1;

    status = fscanf(in_file, "\n");
    if (status < 0)
      return -1;

    // saving value
    if (row.blocks + buffer + 1 <= max_blocks * 2)
      row.arr[j] = buffer;

    if (buffer_buffer != ' ') {
      break;
    }
  }

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
 */
block_arr_t **get_block_data(char *file_name, int *width_ptr,
                             int *height_ptr) {
  // SETUP --------------------------------------------------------------------
  if ((!file_name))
    return NULL;

  FILE *in_file = fopen(file_name, "r");
  if (!in_file)
    return NULL;

  int status;
  int height, width;

  status = fscanf(in_file, "%d %d\n\n", &height, &width);
  if (status < 2) {
    printf("failed to read in width and height\n");
    fclose(in_file);
    return NULL;
  }

  // checking width / height
  //printf("width: %d, height: %d\n", width, height);
  *height_ptr = height;
  *width_ptr = width;
  if ((width > MAX_SIZE) || (width <= 0) ||
      (height > MAX_SIZE) || (height <= 0)) {
    close_vars(in_file, NULL, NULL, NULL, 0, 0);
    return NULL;
  }

  // setting up rows / cols / blocks arrays.
  block_arr_t *rows = calloc(height, sizeof(block_arr_t));
  if (!rows) {
    close_vars(in_file, NULL, NULL, NULL, 0, 0);
    return NULL;
  }

  block_arr_t *cols = calloc(width, sizeof(block_arr_t));
  if (!cols) {
    close_vars(in_file, rows, NULL, NULL, 0, 0);
    return NULL;
  }

  block_arr_t **blocks = malloc(2 * sizeof (block_arr_t *));
  if (!blocks) {
    close_vars(in_file, rows, cols, NULL, 0, 0);
    return NULL;
  }
  blocks[0] = rows;
  blocks[1] = cols;

  // reading rows -------------------------------------------------------------
  unsigned short int buffer;
  for (int i = 0; i < height; i++) {
    status = buffer = fscanf(in_file, "%hu ", &buffer);
    if (status < 1) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
  }
    rows[i].blocks = buffer;
    rows[i].max_block_len = width;
    rows[i].arr = calloc(rows[i].blocks, sizeof(short int));
    if (!(rows[i].arr)) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }
    status = fetch_from_file(in_file, (width + 1)/ 2, rows[i]);
    if (status < 1) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }
  }


  // reading cols -------------------------------------------------------------
  for (int i = 0; i < width; i++) {
    status = buffer = fscanf(in_file, "%hu ", &buffer);
    if (status < 1) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }

    cols[i].blocks = buffer;
    rows[i].max_block_len = width;
    rows[i].arr = calloc(rows[i].blocks, sizeof(short int));
    if (!(rows[i].arr)) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }

    status = fetch_from_file(in_file, (width + 1)/ 2, rows[i]);
    if (status < 1) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }
  }

  return blocks;
}
