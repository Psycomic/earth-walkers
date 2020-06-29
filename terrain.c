#include <stdio.h>

#include "linear_algebra.h"
#include "noise.h"

#define index(x, y, s) (x) * (s) + (y)

void terrain_from_octavien_noise(Vector3* terrain_vertices, float height, float width, uint scale,
				 float* octaves, uint layers, uint size, float frequency, float amplitude) {
  uint real_size = size * scale;

  for (uint x = 0; x < size * scale; ++x) {
    for (uint y = 0; y < size * scale; ++y) {
      float real_x = ((float)x) / scale, real_y = ((float)y) / scale;

      float height_at_point = (noise_octaves_height(octaves, layers, real_x, real_y, frequency, amplitude) + 1) / 2;

      terrain_vertices[x + y * real_size].x = real_x * width;
      terrain_vertices[x + y * real_size].z = real_y * width;
      terrain_vertices[x + y * real_size].y = height_at_point * height;
    }
  }
}

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
