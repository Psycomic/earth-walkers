#include <stdio.h>
#include <stdlib.h>

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

GLFWwindow* opengl_window_create(uint width, uint height, const char* title) {
  /* Initialize GLFW and the opengl context */
  glewExperimental = 1;

  if(!glfwInit()){
    fprintf(stderr, "GLFW not initialized correctly !\n");
    return NULL;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  GLFWwindow* window;
  window = glfwCreateWindow(width, height, title, NULL, NULL);

  if(window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.");
    glfwTerminate();

    return NULL;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    glfwTerminate();

    return NULL;
  }

  /* Enabling depth test and linking the program */
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  return window;
}

GLuint array_buffer_create(uint size, int type, void* data) {
  GLuint array_buffer;
  glGenBuffers(1, &array_buffer);
  glBindBuffer(type, array_buffer);
  glBufferData(type, size, data, GL_STATIC_DRAW);

  return array_buffer;
}

void array_buffer_update(GLuint array_buffer, void* data, uint size) {
  glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void drawable_create(Drawable* destination, Shape* shape, Vector3* color) {
  destination->shape = shape;

  uint data_size = sizeof(Vector3) * shape->vertices_size;
  uint element_size = sizeof(unsigned short) * shape->indices_size;

  destination->vertex_buffer = array_buffer_create(data_size, GL_ARRAY_BUFFER, shape->vertices);
  destination->color_buffer = array_buffer_create(data_size, GL_ARRAY_BUFFER, color);
  destination->index_buffer = array_buffer_create(element_size, GL_ELEMENT_ARRAY_BUFFER, shape->indices);
}

void drawable_update(Drawable* drawable) {
  array_buffer_update(drawable->vertex_buffer, (float*) drawable->shape->vertices,
		      sizeof(Vector3) * drawable->shape->vertices_size);
}

void drawable_draw(Drawable* drawable, GLuint program_id,
		   Mat4 camera_final_matrix, GLuint camera_matrix_id) {
  glUseProgram(program_id);

  glUniformMatrix4fv(camera_matrix_id, 1, GL_FALSE, camera_final_matrix);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, drawable->vertex_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, drawable->color_buffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable->index_buffer);

  // Draw the triangles !
  glDrawElements(
		 GL_TRIANGLES,      // mode
		 drawable->shape->indices_size,    // count
		 GL_UNSIGNED_SHORT,   // type
		 (void*)0           // element array buffer offset
		 );
}
