#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linear_algebra.h"

int read_file(char* buffer, const char* filename);
GLuint load_shaders(const char* vertex_file_path, const char* fragment_file_path);

int main()
{
  Mat4 camera_position_matrix = mat4_allocate();
  Mat4 perspective = mat4_allocate();
  Mat4 final_matrix = mat4_allocate();

  Vector3 position = {1.f, 0.3f, -10.f};
  mat4_create_translation(camera_position_matrix, position);

  mat4_create_perspective(perspective, 100.f, 0.f);

  mat4_mat4_mul(final_matrix, camera_position_matrix, perspective);

  /* Initialize GLFW and the opengl context */
  glewExperimental = 1;

  if(!glfwInit()){
    fprintf(stderr, "GLFW not initialized correctly !\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  GLFWwindow* window;
  window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);

  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window." );
    glfwTerminate();

    return -1;
  }

  glfwMakeContextCurrent(window);
  glewExperimental = 1;

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  /* Initialize the triangle and the shaders */

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  Vector3 cube_vertices[6];
  Vector3 min = {-0.5f, -0.5f, 0.f}, max = {0.5f, 0.5f, 0.f};

  rectangle_vertices_create(cube_vertices, min, max);

  uint vertices_number = sizeof(cube_vertices) / sizeof(Vector3);

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

  GLuint programID = load_shaders("./shaders/vertex_shader_test.glsl", "./shaders/fragment_shader_test.glsl");

  while(!glfwWindowShouldClose(window)){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, final_matrix);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
			  0,
			  3,
			  GL_FLOAT,
			  GL_FALSE,
			  0,
			  (void*)0
			  );

    glDrawArrays(GL_TRIANGLES, 0, vertices_number);
    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }

  return 0;
}
