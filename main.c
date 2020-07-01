#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "misc.h"
#include "render.h"
#include "noise.h"

#define MOUSE_SENSIBILLITY 0.01
#define CAMERA_SPEED 0.5f

#define NOISE_SIZE 5
#define NOISE_OCTAVES 6
#define NOISE_SCALE 40

#define TERRAIN_SIZE ((NOISE_SIZE) * (NOISE_SCALE))

float lerpf(float a, float b, float w) {
  return a + (b - a) * w;
}

float clampf(float x, float min, float max) {
  if (x < min)
    return min;
  else if (x > max)
    return max;

  return x;
}

void update_terrain(Vector3* terrain_vertices, float* octaves[], Drawable* terrain_drawable,
		    float noise_width, float noise_height, float noise_frequency, float noise_amplitude) {
      terrain_from_octavien_noise(terrain_vertices, noise_height, noise_width,
				  NOISE_SCALE, octaves, NOISE_SIZE, NOISE_OCTAVES, noise_frequency, noise_amplitude);

      array_buffer_update(terrain_drawable->vertex_buffer, terrain_vertices,
			  TERRAIN_SIZE * TERRAIN_SIZE * sizeof(Vector3));
}

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

  /* Initializing the noise heightmap */
  const float noise_frequency = 1.7f;

  float noise_width = 15.f;
  float noise_height = 25.f;
  float noise_amplitude = 0.4f;

  float* octaves[NOISE_OCTAVES];
  noise_octaves_create(octaves, NOISE_OCTAVES, NOISE_SIZE, noise_frequency);

  Vector3 lower_terrain_color = {0.f, 0.f, 1.f};
  Vector3 upper_terrain_color = {1.f, 1.f, 1.f};

  Vector3* terrain_color = malloc(TERRAIN_SIZE * TERRAIN_SIZE * sizeof(Vector3));
  for (uint x = 0; x < NOISE_SCALE * NOISE_SIZE; ++x) {
    for (uint y = 0; y < NOISE_SCALE * NOISE_SIZE; ++y) {
      float real_x = ((float)x) / NOISE_SCALE, real_y = ((float)y) / NOISE_SCALE;

      float height_at_point = clampf((noise_octaves_height(octaves, NOISE_SIZE, NOISE_OCTAVES,
							   real_x, real_y,
							   noise_frequency, noise_amplitude) + 1) * 0.7f,
				     0.f, 1.f);

      terrain_color[y * TERRAIN_SIZE + x].x = lerpf(lower_terrain_color.x, upper_terrain_color.x,
						   height_at_point);
      terrain_color[y * TERRAIN_SIZE + x].y = lerpf(lower_terrain_color.y, upper_terrain_color.y,
						   height_at_point);
      terrain_color[y * TERRAIN_SIZE + x].z = lerpf(lower_terrain_color.z, upper_terrain_color.z,
						   height_at_point);
    }
  }

  /* Creating the terrain vertices and shape */
  Vector3* terrain_vertices = malloc(TERRAIN_SIZE * TERRAIN_SIZE * sizeof(Vector3));
  terrain_from_octavien_noise(terrain_vertices, noise_height, noise_width,
			      NOISE_SCALE, octaves, NOISE_SIZE, NOISE_OCTAVES, noise_frequency, noise_amplitude);

  unsigned short terrain_indices[6 * (TERRAIN_SIZE - 1) * (TERRAIN_SIZE - 1)];
  terrain_elements(terrain_indices, TERRAIN_SIZE);

  Shape terrain_shape;
  shape_create(&terrain_shape, terrain_vertices, TERRAIN_SIZE * TERRAIN_SIZE,
	       terrain_indices, sizeof(terrain_indices) / sizeof(unsigned short));

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
    clock_t time_start = clock();

    glClearColor(0.0, 0.0, 0.0, 1.0);
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
    if (glfwGetKey(window, GLFW_KEY_W))
      vector3_add(&camera_position, camera_position, camera_direction);

    if (glfwGetKey(window, GLFW_KEY_S))
      vector3_sub(&camera_position, camera_position, camera_direction);

    if (glfwGetKey(window, GLFW_KEY_J)) {
      noise_width += 1.f;
      update_terrain(terrain_vertices, octaves, &terrain_drawable,
		     noise_width, noise_height, noise_frequency, noise_amplitude);
    }

    if (glfwGetKey(window, GLFW_KEY_K)) {
      noise_width -= 1.f;
      update_terrain(terrain_vertices, octaves, &terrain_drawable,
		     noise_width, noise_height, noise_frequency, noise_amplitude);
    }

    if (glfwGetKey(window, GLFW_KEY_H)) {
      noise_amplitude += 0.01f;
      update_terrain(terrain_vertices, octaves, &terrain_drawable,
		     noise_width, noise_height, noise_frequency, noise_amplitude);
    }

    if (glfwGetKey(window, GLFW_KEY_L)) {
      noise_amplitude -= 0.01f;
      update_terrain(terrain_vertices, octaves, &terrain_drawable,
		     noise_width, noise_height, noise_frequency, noise_amplitude);
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

    clock_t time_end = clock();

    clock_t delta = time_end - time_start;
  }

  glfwTerminate(); // Exit the program
  return 0;
}
