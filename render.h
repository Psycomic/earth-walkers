#ifndef RENDER_HEADER
#define RENDER_HEADER

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linear_algebra.h"

typedef struct {
  Shape* shape;
  GLuint vertex_buffer, color_buffer, index_buffer;
} Drawable;

void camera_create_rotation_matrix(Mat4 destination, float rx, float ry);
void camera_create_final_matrix(Mat4 destination, Mat4 perspective, Mat4 rotation, Vector3 position);

GLFWwindow* opengl_window_create(uint width, uint height, const char* title);

void array_buffer_update(GLuint array_buffer, void* data, uint size);

void drawable_create(Drawable* destination, Shape* shape, Vector3* color);
void drawable_update(Drawable* drawable);
void drawable_draw(Drawable* drawable, GLuint program_id,
		   Mat4 camera_final_matrix, GLuint camera_matrix_id);

#endif
