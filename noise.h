typedef unsigned int uint;

void gradient_initialize(float* gradient, uint size);
float perlin(float* gradient, uint size, float x, float y);
float dot_grid_gradient(float* gradient, uint size, int ix, int iy, float x, float y);

float octaves_height(float* octaves, uint size, float x, float y, float frequency, float amplitude);
