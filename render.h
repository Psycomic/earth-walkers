#ifndef RENDER_HEADER
#define RENDER_HEADER

#include <GL/glew.h>

#include "linear_algebra.h"

void camera_create_rotation_matrix(Mat4 destination, float rx, float ry);
void camera_create_final_matrix(Mat4 destination, Mat4 perspective, Mat4 rotation, Vector3 position);

GLuint create_array_buffer(uint size);
void update_array_buffer(GLuint array_buffer, float* data, uint size);

void shape_draw(GLuint vertex_buffer, GLuint color_buffer, GLuint program_id,
		Mat4 camera_final_matrix, GLuint camera_matrix_id, uint vertices_number);
void shape_apply_transform(ConvexShape* shape, GLuint vertex_buffer, Mat4 transform);

#endif
