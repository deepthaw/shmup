#include <stdio.h>
#include <stdlib.h>

int main(int n, char* args[]) {
  int dim = 2;
  char*** strings = malloc(dim *sizeof(char**));
  for (int i = 0; i != dim; i++) {
    strings[i] = malloc(dim * sizeof(char *));
    for (int j = 0; j != dim; j++) {
      strings[i][j] = malloc(sizeof(char));
    }
  }


  strings[0][0] = "test";

  printf("%s\n", strings[0][0]);



  return 0;
}
