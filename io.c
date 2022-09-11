#include "io.h"

static unsigned short int WIDTH = 0;
static unsigned short int HEIGHT = 0;

/*
 * called by get_block_data. It fetches the data from the file.
 *
 *
 */
static int fetch_from_file(FILE *in_file,
                           clue_storage_t *blocks, bool is_row) {

  if ((!in_file) || (!blocks))
    return -1;

  if ((!(blocks->rows)) || (!blocks->columns))
    return -1;

  int status;
  unsigned short int buffer;
  clue_arr block_arr[WIDTH][HEIGHT] = ((is_row) ? (blocks->rows):(blocks->columns));

  printf("malloc good. (%lu)\n", sizeof(*blocks));

  // reading in rows
  for (int j = 0; j < HEIGHT; j++) {
    int row_sum = -1;
    int num_blocks = 0;

    printf("1.\n");
    // finding the number of blocks in the row
    status = fscanf(in_file, "%d ", &num_blocks);
    if (status < 1) {
      fclose(in_file);
      free(blocks);
      return FILE_ERROR;
    }

    printf("2.\n");
    printf("number: %d, ", num_blocks);
    fflush(NULL);

    // reading in blocks
    for (int k = 0; k < num_blocks; k++) {
      status = fscanf(in_file, "%hu ", &buffer);
      if ((status < 1)) {
        fclose(in_file);
        return FILE_ERROR;
      }

      printf("3. ");
      fflush(NULL);

      printf("%hu", buffer);
      fflush(NULL);
      // saving value
     if (row_sum + buffer + 1 <= WIDTH) {
        printf("%hu (%d, %d)", buffer, j, k);
        fflush(NULL);
        *(block_arr[j][k]) = buffer;
        row_sum += buffer + 1;
      }
    }

    status = fscanf(in_file, "\n");
    printf("\n");
  }
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
 * [row 1 # of blocks] [block 1 length] [block 2 length]
 * [row 2 # of blocks] [block 1 length] ...
 * ...
 * [row n # of blocks] ...
 *
 * [column 1 # of blocks] [block 1 length]...
 * ...
 * [column n # of blocks]
 *
 * note, the total length (and spaces between each block) can't be longer than
 * the length of that row / length.
 *
 */
clue_storage_t *get_block_data(char *file_name) {

  if ((!file_name))
    return NULL;

  FILE *in_file;

  //initalizing FILE
  in_file = fopen(file_name, "r");

  if (!in_file)
    return NULL;

  int status;


  //getting dimensions of nonogram
  status = fscanf(in_file, "%hu %hu\n", &HEIGHT, &WIDTH);

  if ((status < 2) || (HEIGHT > MAX_SIZE) || (WIDTH > MAX_SIZE)) {
    printf("failed to read in width and height\n");
    fclose(in_file);
    return NULL;
  }

  printf("width: %d, height: %d\n", WIDTH, HEIGHT);



  // mallocing row blocks ()
  clue_storage_t *blocks = malloc(sizeof(clue_storage_t));



  status = fetch_from_file(in_file, blocks, true);
  if (status < 1) {
    fclose(in_file);
    return NULL;
  }



  /*

  `  */

  return blocks;
}

int main(int argc, char *argv[]) {
  clue_storage_t *blocks = get_block_data("blocks.txt");
  if (!blocks)
    return -1;

  printf("block fetch good: %lu", sizeof(*blocks));
  free(blocks);
}


