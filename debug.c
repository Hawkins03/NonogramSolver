#include "io.h"

int main(int argc, char *argv[]) {
  int height, width;
  block_arr_t **blocks = get_block_data("blocks.txt", &height, &width);
  if (!blocks)
    return -1;

  printf("Function good.\n");
  printf("width = %d, height = %d\n", width, height);

  for (int i = 0; i < height; i++) {
    printf("%p, ", blocks[0][i].arr);
    printf("%hu: ", blocks[0][i].blocks);
    for (int k = 0; k < blocks[0][i].blocks; k++) {
      printf("%hu ", blocks[0][i].arr[k]);
    }
    printf("\n");
  }
  printf("\n");

  for (int i = 0; i < width; i++) {
    printf("%p, ", &blocks[1][i].arr);
    printf("%hu: ", blocks[1][i].blocks);
    fflush(NULL);
    if (blocks[1][i].arr) {
      for (int k = 0; k < blocks[1][i].blocks; k++) {
        printf("%hu ", blocks[1][i].arr[k]);
      }
      printf("\n");
    }
  }
  printf("\n");
  free(blocks);
}
