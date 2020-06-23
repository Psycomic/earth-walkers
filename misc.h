#include <GL/glew.h>

typedef unsigned int uint;

GLuint load_shaders(const char* vertex_file_path, const char* fragment_file_path);
int read_file(char* buffer, const char* filename);
float randomf();
void random_arrayf(float* destination, uint size);
