#include "io.h"

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
