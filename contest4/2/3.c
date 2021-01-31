#include <stdio.h>
#include <stdlib.h>

extern void mergesort(int from, int to, const int *in, int *out);



int main() {
  int from = 0;
  int to = 5;
  int *in = (int*) malloc(to*sizeof(int));
  int *out = (int*) malloc(to*sizeof(int));

  for (int i = 0; i < to; ++i) {
    in[i] = to - i;
  }

  mergesort(from, to, in, out);

  for (int i = 0; i < to - from; ++i) {
    printf("%d ", in[i]);
  }
  printf("\n");

  return 0;
}
