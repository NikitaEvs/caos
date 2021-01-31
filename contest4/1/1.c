#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <float.h>


double my_sin_real(double x) {
    int k = 0;
    double addition = x;
    double result = 0;

    while (addition > 0) {
        if (k % 2 == 0) {
            result += addition;
        } else {
            result -= addition;
        }

        ++k;
        addition *= x * x;
        addition /= (1 + 2*k)*2*k;
    }
    return result;
}

extern double my_sin(double x);


int main() {

  for (double x = -0.1; x < 0.1; x = x + 0.00000001) {
    if (fabs(sin(x) - my_sin(x)) > DBL_EPSILON) {
      printf("%.10lf Out: %.20lf Real: %.20lf \n", x, my_sin(x), sin(x));
      return 0;
    }
  }
  printf("%s\n", "Great");

  return 0;
}
