#ifndef NOISE_HEADER
#define NOISE_HEADER

typedef unsigned int uint;

void noise_gradient_initialize(float* gradient, uint size);
float noise_perlin(float* gradient, uint size, float x, float y);

float noise_octaves_height(float* octaves, uint size,
			   float x, float y, float frequency, float amplitude);

#endif
