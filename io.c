#include "io.h"

/*
 * function called by get_block_data for each seperate line input.
 */
static int fetch_from_file(FILE *in_file,
                           block_arr_t *row) {
  if (!row)
    return -1;

  int status;
  unsigned short int buffer;

  // reading in blocks
  for (int j = 0; j < row->blocks; j++) {
    status = fscanf(in_file, " %hu", &buffer);
    if (status < 1)
      return -1;

    // saving value
    if (row->blocks + buffer - 1 <= row->max_block_len)
      row->arr[j] = buffer;
  }

  return 1;
}

/*
 * Pulls in data from the specified file. (width_ptr and height_ptr act as
 * extra return values). See README.md for the format.
 */
block_arr_t **get_block_data(char *file_name, int *width_ptr, int *height_ptr) {
  if ((!file_name))
    return NULL;

  FILE *in_file = fopen(file_name, "r");
  if (!in_file)
    return NULL;

  int status;
  int height, width;
  status = fscanf(in_file, "%d %d\n\n", &height, &width);
  if (status < 2) {
    fclose(in_file);
    in_file = NULL;
    return NULL;
  }

  if ((width > MAX_SIZE) || (width <= 0) ||
      (height > MAX_SIZE) || (height <= 0)) {
    close_vars(in_file, NULL, NULL, NULL, 0, 0);
    return NULL;
  }

  *height_ptr = height;
  *width_ptr = width;

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
  blocks[0] = rows;
  blocks[1] = cols;
  if (!blocks) {
    close_vars(in_file, rows, cols, NULL, 0, 0);
    return NULL;
  }


  unsigned short int buffer;
  for (int i = 0; i < height; i++) {
    buffer = 0;
    status = fscanf(in_file, " %hu", &buffer);
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

    status = fetch_from_file(in_file, &rows[i]);
    if (status < 1) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }
    status = fscanf(in_file, "\n");
  }
  status = fscanf(in_file, "\n");

  // reading cols -------------------------------------------------------------
  for (int i = 0; i < width; i++) {
    status = fscanf(in_file, "\n %hu ", &buffer);
    if (status < 1) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }

    cols[i].blocks = buffer;
    cols[i].max_block_len = height;
    cols[i].arr = calloc(cols[i].blocks, sizeof(short int));
    if (!(cols[i].arr)) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }

    status = fetch_from_file(in_file, &cols[i]);
    if (status < 1) {
      close_vars(in_file, rows, cols, blocks, height, width);
      return NULL;
    }
  }

  return blocks;
}

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

