#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "render.h"

#define MOUSE_SENSIBILLITY 0.01
#define CAMERA_SPEED 0.1f

double last_xpos = -1.0, last_ypos = -1.0;

int main()
{
  float camera_perspective_matrix[16];
  float camera_rotation_matrix[16];
  float camera_final_matrix[16];

  mat4_create_perspective(camera_perspective_matrix, 100.f, 0.f);

  Vector3 camera_position = {0.f, 0.3f, -4.f};
  float camera_rx = 0.f, camera_ry = 0.f;

  Vector4 camera_orientation = {0.f, 0.f, 1.f, 0.f};

  Vector3 cube_vertices[36];
  vertices_create_box(cube_vertices, 1.f, 1.f, 1.f);

  uint vertices_number = sizeof(cube_vertices) / sizeof(Vector3);

  /* Initialize GLFW and the opengl context */
  if(initialize_glfw() == -1) {
    fprintf( stderr, "Failed to initialize glfw" );

    return -1;
  }

  GLFWwindow* window;
  window = glfwCreateWindow(800, 800, "Tutorial 01", NULL, NULL);

  if(window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.");
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

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

  GLuint programID = load_shaders("./shaders/vertex_shader_test.glsl", "./shaders/fragment_shader_test.glsl");
  glUseProgram(programID);

  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  while(!glfwWindowShouldClose(window)) {
    /* Updating camera position */
    camera_create_rotation_matrix(camera_rotation_matrix, camera_rx, camera_ry);
    camera_create_final_matrix(camera_final_matrix, camera_perspective_matrix,
			       camera_rotation_matrix, camera_position);

    /* Calculating the forward vector */
    Vector4 camera_direction_vec4;
    mat4_vector4_mul(&camera_direction_vec4, camera_orientation, camera_rotation_matrix);

    Vector3 camera_direction;
    camera_direction = *((Vector3*) &camera_direction_vec4);

    vector3_scalar_mul(&camera_direction, camera_direction, CAMERA_SPEED);

    /* Drawing the triangles */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, camera_final_matrix);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, vertices_number);
    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();

    /* Handling user input */
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_W))
      vector3_add(&camera_position, camera_position, camera_direction);
    if (glfwGetKey(window, GLFW_KEY_S))
      vector3_sub(&camera_position, camera_position, camera_direction);


    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if(last_xpos == -1.0 || last_ypos == -1.0){
      last_xpos = xpos;
      last_ypos = ypos;
    }

    camera_rx += (ypos - last_ypos) * MOUSE_SENSIBILLITY;
    camera_ry -= (xpos - last_xpos) * MOUSE_SENSIBILLITY;

    last_xpos = xpos;
    last_ypos = ypos;
  }

  return 0;
}
