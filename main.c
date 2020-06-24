#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linear_algebra.h"
#include "misc.h"
#include "render.h"

extern float camera_final_matrix[16];
extern float shape_cube_vertices[108];

extern GLuint cube_shape_vertex_buffer, cube_shape_fragment_buffer;

void game_init();
void game_loop();
void game_handle_events(GLFWwindow* window);

int main()
{
  srand((uint) time(NULL));

  float test_matrix[16];
  mat4_create_rotation_x(test_matrix, 0.05f);

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

  /* Initialize the triangle and the shaders */
  game_init();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  float cube_color[108];
  random_arrayf(cube_color, 108);

  Vector3 cube_vertices[36];
  memcpy(cube_vertices, shape_cube_vertices, sizeof(shape_cube_vertices));

  ConvexShape shape;
  convex_shape_create(&shape, cube_vertices, 36);

  uint vertices_size = sizeof(shape_cube_vertices) / sizeof(float);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  GLuint vertexbuffer = create_array_buffer(sizeof(cube_vertices));
  GLuint colorbuffer = create_array_buffer(sizeof(cube_vertices));

  update_array_buffer(colorbuffer, cube_color, sizeof(cube_vertices));

  GLuint programID = load_shaders("./shaders/vertex_shader_test.glsl", "./shaders/fragment_shader_test.glsl");

  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  while(!glfwWindowShouldClose(window)) {
    game_loop();

    vector3_apply_transform(test_matrix, shape.vertices, 36);
    update_array_buffer(vertexbuffer, (float*) shape.vertices, sizeof(shape_cube_vertices));

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Drawing the triangles */
    shape_draw(vertexbuffer, colorbuffer, programID, camera_final_matrix, MatrixID, vertices_size / 3);
    shape_draw(cube_shape_vertex_buffer, cube_shape_fragment_buffer, programID,
	       camera_final_matrix, MatrixID, vertices_size / 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    game_handle_events(window);
  }

  glfwTerminate();
  return 0;
}
