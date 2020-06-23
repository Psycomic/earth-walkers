#ifndef RENDER_HEADER
#define RENDER_HEADER

#include <GL/glew.h>

#include "linear_algebra.h"

typedef struct {
  uint vertices_number;

  GLuint vertex_buffer;
  GLuint color_buffer;

  Mat4 transform;
} Shape;

void camera_create_rotation_matrix(Mat4 destination, float rx, float ry);
void camera_create_final_matrix(Mat4 destination, Mat4 perspective, Mat4 rotation, Vector3 position);

GLuint create_array_buffer(float* data, uint size);

void shape_create(Shape* shape, float* vertices, float* colors, uint vertices_number);
void shape_draw(Shape* shape, Mat4 camera_final_matrix, GLuint program_id,
		GLuint camera_matrix_id, GLuint transform_id);

#endif
