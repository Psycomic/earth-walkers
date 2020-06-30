#ifndef TERRAIN_HEADER
#define TERRAIN_HEADER

#include "linear_algebra.h"

void terrain_from_octavien_noise(Vector3* terrain_vertices, float height, float width, uint scale,
				 float* octaves[], uint layers, uint size, float frequency, float amplitude);
void terrain_elements(unsigned short* elements, uint terrain_size);

#endif
