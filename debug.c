#include "io.h"

int main(int argc, char *argv[]) {
  int height, width;
  block_arr_t **blocks = get_block_data("blocks.txt", &height, &width);
  if (!blocks)
    return -1;

  printf("Function good.\n");
  printf("width = %d, height = %d\n", width, height);

  for (int j = 0; j < height; j++) {
    for (int k = 0; k < width; k++) {
      printf("%hu ", blocks[0][j].arr[k]);
    }
    printf("\n");
  }
  printf("\n");

  for (int j = 0; j < width; j++) {
    for (int k = 0; k < height; k++) {
      printf("%hu ", blocks[1][j].arr[k]);
    }
    printf("\n");
  }
  printf("\n");
  free(blocks);
}
