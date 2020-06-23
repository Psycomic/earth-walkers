#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "render.h"

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

GLuint create_array_buffer(float* data, uint size) {
  GLuint array_buffer;
  glGenBuffers(1, &array_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

  return array_buffer;
}

void shape_create(Shape* shape, float* vertices, float* colors, uint vertices_number) {
  shape->vertex_buffer = create_array_buffer(vertices, vertices_number);
  shape->color_buffer = create_array_buffer(colors, vertices_number);

  shape->vertices_number = vertices_number;
}

void shape_draw(Shape* shape, Mat4 camera_final_matrix, GLuint program_id,
		GLuint camera_matrix_id, GLuint transform_id) {
  glUseProgram(program_id);

  glUniformMatrix4fv(camera_matrix_id, 1, GL_FALSE, camera_final_matrix);
  glUniformMatrix4fv(transform_id, 1, GL_FALSE, shape->transform);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, shape->vertex_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, shape->color_buffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glDrawArrays(GL_TRIANGLES, 0, shape->vertices_number);
}
