#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "render.h"

float shape_cube_vertices[] = {
			 -1.0f,-1.0f,-1.0f, // triangle 1 : begin
			 -1.0f,-1.0f, 1.0f,
			 -1.0f, 1.0f, 1.0f, // triangle 1 : end
			 1.0f, 1.0f,-1.0f, // triangle 2 : begin
			 -1.0f,-1.0f,-1.0f,
			 -1.0f, 1.0f,-1.0f, // triangle 2 : end
			 1.0f,-1.0f, 1.0f,
			 -1.0f,-1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 -1.0f,-1.0f,-1.0f,
			 -1.0f,-1.0f,-1.0f,
			 -1.0f, 1.0f, 1.0f,
			 -1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f, 1.0f,
			 -1.0f,-1.0f, 1.0f,
			 -1.0f,-1.0f,-1.0f,
			 -1.0f, 1.0f, 1.0f,
			 -1.0f,-1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,
			 -1.0f, 1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			 -1.0f, 1.0f,-1.0f,
			 -1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 -1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f
};

void camera_create_rotation_matrix(Mat4 destination, float rx, float ry) {
  float rotation_matrix_x[16];
  float rotation_matrix_y[16];

  mat4_create_rotation_x(rotation_matrix_x, rx);
  mat4_create_rotation_y(rotation_matrix_y, ry);

  mat4_mat4_mul(destination, rotation_matrix_y, rotation_matrix_x); /* First, Y rotation, after X rotation */
}

void camera_create_final_matrix(Mat4 destination, Mat4 perspective, Mat4 rotation, Vector3 position) {
  float translation_matrix[16];
  float temporary_matrix[16];

  mat4_create_translation(translation_matrix, position);

  mat4_mat4_mul(temporary_matrix, translation_matrix, rotation);
  mat4_mat4_mul(destination, temporary_matrix, perspective);
}

GLuint create_array_buffer(uint size) {
  GLuint array_buffer;
  glGenBuffers(1, &array_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

  return array_buffer;
}

void update_array_buffer(GLuint array_buffer, float* data, uint size) {
  glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void drawable_create(Drawable* destination, ConvexShape* shape, Vector3* color) {
  destination->shape = shape;

  uint data_size = sizeof(Vector3) * shape->vertices_size;

  destination->vertex_buffer = create_array_buffer(data_size);
  destination->color_buffer = create_array_buffer(data_size);
  update_array_buffer(destination->color_buffer, (float*) color, data_size);
}

void drawable_update(Drawable* drawable, Mat4 transform) {
    convex_shape_apply_transform(drawable->shape, transform);
    update_array_buffer(drawable->vertex_buffer, (float*) drawable->shape->vertices,
			sizeof(Vector3) * drawable->shape->vertices_size);

}

void shape_draw(Drawable* drawable, GLuint program_id,
		Mat4 camera_final_matrix, GLuint camera_matrix_id) {
  glUseProgram(program_id);

  glUniformMatrix4fv(camera_matrix_id, 1, GL_FALSE, camera_final_matrix);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, drawable->vertex_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, drawable->color_buffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glDrawArrays(GL_TRIANGLES, 0, drawable->shape->vertices_size);
}
