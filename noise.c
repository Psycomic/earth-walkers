#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include "linear_algebra.h"

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

float noise_octaves_height(float* octaves[], uint size, uint count,
			   float x, float y, float frequency, float amplitude){
  float final_height = 0.f;

  for (uint i = 0; i < count; i++) {
    float scale = powf(frequency, (float) i);
    float importance = powf(amplitude, (float) i);

    uint octave_size = size * (uint) scale;

    final_height += noise_perlin(octaves[i], octave_size, x * scale, y * scale) * importance;
  }

  return final_height;
}

void terrain_from_octavien_noise(Vector3* terrain_vertices, float height, float width, uint scale,
				 float* octaves[], uint size, uint layers, float frequency, float amplitude) {
  uint real_size = size * scale;

  for (uint x = 0; x < size * scale; ++x) {
    for (uint y = 0; y < size * scale; ++y) {
      float real_x = ((float)x) / scale, real_y = ((float)y) / scale;

      float height_at_point = (noise_octaves_height(octaves, size, layers,
						    real_x, real_y, frequency, amplitude) + 1) / 2;

      terrain_vertices[x + y * real_size].x = real_x * width;
      terrain_vertices[x + y * real_size].z = real_y * width;
      terrain_vertices[x + y * real_size].y = height_at_point * height;
    }
  }
}

#undef index
#define index(x, y, s) (x) * (s) + (y)

void terrain_elements(unsigned short* elements, uint terrain_size) {
  for (uint x = 0; x < terrain_size - 1; ++x) {
    for (uint y = 0; y < terrain_size - 1; ++y) {
      register uint p = x * (terrain_size - 1) * 6 + y * 6;

      elements[p + 0] = index(x, y, terrain_size);
      elements[p + 1] = index(x + 1, y, terrain_size);
      elements[p + 2] = index(x, y + 1, terrain_size);
      elements[p + 3] = index(x + 1, y, terrain_size);
      elements[p + 4] = index(x, y + 1, terrain_size);
      elements[p + 5] = index(x + 1, y + 1, terrain_size);
    }
  }
}
