#define _USE_MATH_DEFINES
#include <math.h>

#include <stdlib.h>

#define index(x, y, z, size) x + y * size + z * size * size

typedef unsigned int uint;

float lerp(float a0, float a1, float w) {
  float mu2;

  mu2 = (1.f - cosf(w * M_PI)) / 2;
  return(a0 * (1 - mu2) + a1 * mu2);
}

float random_float() {
  return ((float)rand()) / RAND_MAX;
}

float random_uniform(float min, float max) {
  return min + (max - min) * random_float();
}

void noise_gradient_initialize(float* gradient, uint size) {
  for (size_t i = 0; i < size; i += 2){
    gradient[i] = random_uniform(-1.f, 1.f);
    gradient[i + 1] = random_uniform(-1.f, 1.f);
  }
}

float dot_grid_gradient(float* gradient, uint size, int ix, int iy, float x, float y) {
  float dx = x - (float)ix;
  float dy = y - (float)iy;

  return (dx * gradient[index(ix, iy, 0, size)] +
	  dy * gradient[index(ix, iy, 1, size)]);
}

float noise_perlin(float* gradient, uint size, float x, float y) {
  int x0 = (int)x;
  int x1 = x0 + 1;
  int y0 = (int)y;
  int y1 = y0 + 1;

  float sx = x - (float)x0;
  float sy = y - (float)y0;

  float n0, n1, ix0, ix1, value;

  n0 = dot_grid_gradient(gradient, size, x0, y0, x, y);
  n1 = dot_grid_gradient(gradient, size, x1, y0, x, y);
  ix0 = lerp(n0, n1, sx);

  n0 = dot_grid_gradient(gradient, size, x0, y1, x, y);
  n1 = dot_grid_gradient(gradient, size, x1, y1, x, y);
  ix1 = lerp(n0, n1, sx);

  value = lerp(ix0, ix1, sy);
  return value;
}

void noise_octaves_create(float* octaves[], uint count, uint size, float frequency) {
  for (uint i = 0; i < count; ++i) {
    uint octave_size = size * ((uint) powf(frequency, (float)i) + 1);

    octaves[i] = malloc(sizeof(float) * octave_size * octave_size * 2);
    noise_gradient_initialize(octaves[i], octave_size * octave_size * 2);
  }
}

float noise_octaves_height(float* octaves[], uint size,
			   float x, float y, float frequency, float amplitude){
  float final_height = 0.f;

  for (uint i = 0; i < size; i++) {
    float scale = powf(frequency, (float) i);
    float importance = powf(amplitude, (float) i);

    uint octave_size = size * (uint) scale;

    final_height += noise_perlin(octaves[i], octave_size, x * scale, y * scale) * importance;
  }

  return final_height;
}
