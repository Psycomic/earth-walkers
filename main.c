#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linear_algebra.h"
#include "misc.h"
#include "render.h"

#define MOUSE_SENSIBILLITY 0.01
#define CAMERA_SPEED 0.1f

extern float shape_cube_vertices[108];

int main()
{
  srand((uint) time(NULL));  // Seed for random number generation

  double last_xpos = -1.0, last_ypos = -1.0;

  float camera_rotation_matrix[16];
  float camera_final_matrix[16];

  float rotation_matrix[16];

  Vector3 camera_orientation = {0.f, 0.f, 1.f};
  Vector3 camera_position = {0.f, 0.3f, -4.f};
  float camera_rx = 0.f, camera_ry = 0.f;

  Vector3 camera_direction;

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

  /* Creating perspective matrix */
  float camera_perspective_matrix[16];
  mat4_create_perspective(camera_perspective_matrix, 100.f, 0.1f);
  mat4_create_rotation_x(rotation_matrix, 0.01f);

  /* Creating the ConvexShape and the Drawable object */
  Vector3 cube1_shape_color[36];
  random_arrayf((float*) cube1_shape_color, 108);

  Vector3 cube1_shape_vertices[36];
  memcpy(cube1_shape_vertices, shape_cube_vertices, sizeof(shape_cube_vertices));

  ConvexShape cube1_shape;
  Drawable cube1_drawable;
  convex_shape_create(&cube1_shape, cube1_shape_vertices, 36);
  drawable_create(&cube1_drawable, &cube1_shape, cube1_shape_color);

  Vector3 cube2_shape_color[36];
  random_arrayf((float*) cube2_shape_color, 108);

  Vector3 cube2_shape_vertices[36];
  memcpy(cube2_shape_vertices, shape_cube_vertices, sizeof(shape_cube_vertices));

  ConvexShape cube2_shape;
  Drawable cube2_drawable;
  convex_shape_create(&cube2_shape, cube2_shape_vertices, 36);
  drawable_create(&cube2_drawable, &cube2_shape, cube2_shape_color);

  /* Enabling depth test and linking the program */
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  GLuint program_id = load_shaders("./shaders/vertex_shader_test.glsl", "./shaders/fragment_shader_test.glsl");
  GLuint matrix_id = glGetUniformLocation(program_id, "MVP");

  while(!glfwWindowShouldClose(window)) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the screen (color: white)

    /* Calculating the camera's forward vector */
    mat4_vector3_mul(&camera_direction, camera_orientation, camera_rotation_matrix);
    vector3_scalar_mul(&camera_direction, camera_direction, CAMERA_SPEED);

    /* Updating camera position */
    camera_create_rotation_matrix(camera_rotation_matrix, camera_rx, camera_ry);
    camera_create_final_matrix(camera_final_matrix, camera_perspective_matrix,
			       camera_rotation_matrix, camera_position);

    /* Rotating and drawing the cube */
    drawable_transform(&cube1_drawable, rotation_matrix);

    drawable_draw(&cube1_drawable, program_id, camera_final_matrix, matrix_id);
    drawable_draw(&cube2_drawable, program_id, camera_final_matrix, matrix_id);

    /* Testing collision */
    printf("Is colliding ? %d\n", convex_shape_shape_collide(&cube1_shape, &cube2_shape));

    glfwSwapBuffers(window);
    glfwPollEvents();

    /* Keyboard input handling */
    if (glfwGetKey(window, GLFW_KEY_W)) {
      vector3_add(&camera_position, camera_position, camera_direction);

      for (uint i = 0; i < cube2_shape.vertices_size; ++i) {
	vector3_sub(cube2_shape.vertices + i, cube2_shape.vertices[i], camera_direction);
      }

      drawable_transform(&cube2_drawable, NULL);
    }

    if (glfwGetKey(window, GLFW_KEY_S)) {
      vector3_sub(&camera_position, camera_position, camera_direction);

      for (uint i = 0; i < cube2_shape.vertices_size; ++i) {
	vector3_add(cube2_shape.vertices + i, cube2_shape.vertices[i], camera_direction);
      }

      drawable_transform(&cube2_drawable, NULL);
    }

    /* Mouse cursor handling */
    {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);

      if(last_xpos == -1.0 || last_ypos == -1.0){
	last_xpos = xpos;
	last_ypos = ypos;
      }

      /* Rotate the camera proporitionaly to the mouse position */
      camera_rx += (ypos - last_ypos) * MOUSE_SENSIBILLITY;
      camera_ry -= (xpos - last_xpos) * MOUSE_SENSIBILLITY;

      last_xpos = xpos;
      last_ypos = ypos;
    }
  }

  glfwTerminate(); // Exit the program
  return 0;
}
