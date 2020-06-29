#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "misc.h"
#include "render.h"
#include "noise.h"
#include "terrain.h"

#define MOUSE_SENSIBILLITY 0.01
#define CAMERA_SPEED 0.1f

#define NOISE_SIZE 5
#define NOISE_OCTAVES 5
#define NOISE_SCALE 20

#define TERRAIN_SIZE ((NOISE_SIZE) * (NOISE_SCALE))

int main()
{
  srand((uint) time(NULL));  // Seed for random number generation

  double last_xpos = -1.0, last_ypos = -1.0;

  float camera_rotation_matrix[16];
  float camera_final_matrix[16];

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
  mat4_create_perspective(camera_perspective_matrix, 1000.f, 0.1f);

  /* Creating the terrain mesh */
  float gradient[2 * NOISE_SIZE * NOISE_SIZE * NOISE_OCTAVES];
  noise_gradient_initialize(gradient, sizeof(gradient) / sizeof(float));

  Vector3 terrain_vertices[TERRAIN_SIZE * TERRAIN_SIZE];
  terrain_from_octavien_noise(terrain_vertices, 50.f, 20.f,
			      NOISE_SCALE, gradient, NOISE_OCTAVES, NOISE_SIZE, 0.5f, 0.3f);

  unsigned short terrain_indices[6 * (TERRAIN_SIZE - 1) * (TERRAIN_SIZE - 1)];
  terrain_elements(terrain_indices, TERRAIN_SIZE);

  Shape terrain_shape;
  shape_create(&terrain_shape, terrain_vertices, sizeof(terrain_vertices) / sizeof(Vector3),
	       terrain_indices, sizeof(terrain_indices) / sizeof(unsigned short));

  Vector3 terrain_color[TERRAIN_SIZE * TERRAIN_SIZE];
  random_arrayf((float*) terrain_color, sizeof(terrain_color) / (sizeof(float)));

  Drawable terrain_drawable;
  drawable_create(&terrain_drawable, &terrain_shape, terrain_color);

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

    drawable_draw(&terrain_drawable, program_id, camera_final_matrix, matrix_id); /* Drawing the terrain */

    glfwSwapBuffers(window);
    glfwPollEvents();

    /* Keyboard input handling */
    if (glfwGetKey(window, GLFW_KEY_W)) {
      vector3_add(&camera_position, camera_position, camera_direction);
    }

    if (glfwGetKey(window, GLFW_KEY_S)) {
      vector3_sub(&camera_position, camera_position, camera_direction);
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
