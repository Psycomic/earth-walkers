#include <GL/glew.h>

#include "linear_algebra.h"

int read_file(char* buffer, const char* filename);
GLuint load_shaders(const char* vertex_file_path, const char* fragment_file_path);

void camera_create_rotation_matrix(Mat4 destination, float rx, float ry);
void camera_create_final_matrix(Mat4 destination, Mat4 perspective, Mat4 rotation, Vector3 position);
