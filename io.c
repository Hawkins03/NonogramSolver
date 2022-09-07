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
 */
int get_block_data(char *file_name,
                   unsigned short *rows[MAX_SIZE][MAX_SIZE / 2],
                   unsigned short *columns[MAX_SIZE][MAX_SIZE / 2]) {
  printf("NOT DONE YET");

  return 1;
}

//regex = "(((\\d+) ?)+\n?)+\n(((\\d+) ?)+\n?)+";
