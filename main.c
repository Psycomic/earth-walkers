#include <stdio.h>
#include <stdlib.h>

#include "Noise.h"

int main()
{
  uint size = 20;

  float* gradient = (float*) malloc(sizeof(float) * size * size * 2);
  gradient_initialize(gradient, size * size * 2);

  float height_at_point1 = perlin(gradient, size, 3.1f, 3.2f);
  float height_at_point2 = perlin(gradient, size, 3.1f, 3.5f);

  printf("Hello world : %f %f\n", height_at_point1, height_at_point2);
  return 0;
}
