#ifndef NOISE_HEADER
#define NOISE_HEADER

#include "linear_algebra.h"

typedef unsigned int uint;

float random_float();

void noise_gradient_initialize(float* gradient, uint size);
float noise_perlin(float* gradient, uint size, float x, float y);

void noise_octaves_create(float* octaves[], uint count, uint size, float frequency);
float noise_octaves_height(float* octaves[], uint size, uint count,
			   float x, float y, float frequency, float amplitude);

void terrain_from_octavien_noise(Vector3* terrain_vertices, float height, float width, uint scale,
				 float* octaves[], uint size, uint layers, float frequency, float amplitude);
void terrain_elements(unsigned short* elements, uint terrain_size);

#endif
