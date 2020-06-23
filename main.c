#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linear_algebra.h"
#include "misc.h"
#include "render.h"

extern float camera_final_matrix[16];

void game_init();
void game_loop();
void game_handle_events(GLFWwindow* window);

int main()
{
  srand((uint) time(NULL));
  game_init();

  float cube_color[108];
  random_arrayf(cube_color, 108);

  float cube_vertices[] = {
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

  uint vertices_number = sizeof(cube_vertices) / sizeof(float);

  Vector3 cube_rotation = {0.f, 0.f, 0.f};
  float cube_matrix[16];

  Vector3 test_position = {-3.f, 0.f, 0.f};
  float test_matrix[16];
  mat4_create_translation(test_matrix, test_position);

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
  window = glfwCreateWindow(800, 800, "Tutorial 01", NULL, NULL);

  if(window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.");
    glfwTerminate();

    return -1;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  /* Initialize the triangle and the shaders */

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  GLuint vertexbuffer = create_array_buffer(cube_vertices, sizeof(cube_vertices));
  GLuint colorbuffer = create_array_buffer(cube_color, sizeof(cube_color));

  Shape rotating_cube_shape = {vertices_number, vertexbuffer, colorbuffer, cube_matrix};
  Shape test_cube_shape = {vertices_number, vertexbuffer, colorbuffer, test_matrix};

  GLuint programID = load_shaders("./shaders/vertex_shader_test.glsl", "./shaders/fragment_shader_test.glsl");

  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  GLuint PositionMatrixID = glGetUniformLocation(programID, "position_mat4");

  while(!glfwWindowShouldClose(window)) {
    game_loop();

    mat4_create_rotation_x(cube_matrix, cube_rotation.x);

    cube_rotation.x += 0.01f;

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Drawing the triangles */
    shape_draw(&rotating_cube_shape, camera_final_matrix, programID, MatrixID, PositionMatrixID);
    shape_draw(&test_cube_shape, camera_final_matrix, programID, MatrixID, PositionMatrixID);

    glfwSwapBuffers(window);
    glfwPollEvents();

    game_handle_events(window);
  }

  glfwTerminate();
  return 0;
}
