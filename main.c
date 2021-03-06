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
#include "physics.h"

#define MOUSE_SENSIBILLITY 0.01f
#define CAMERA_SPEED 0.1f

#define NOISE_SIZE 5
#define NOISE_OCTAVES 6
#define NOISE_SCALE 40

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

  Vector3 gravity = {0.f, -0.001f, 0.f};

  Vector3 cube_direction = {0.f, 0.f, 0.f};

  /* Creating the floor and the cube shapes */
  float cube_vertices[] = {
			     1,-1, 1,
			     1,-1,-1,
			     1, 1,-1,
			     1, 1, 1,
			     -1,-1, 1,
			     -1,-1,-1,
			     -1, 1,-1,
			     -1, 1, 1
  };

  unsigned short cube_elements[] = {
				    4, 0, 3,
				    4, 3, 7,
				    0, 1, 2,
				    0, 2, 3,
				    1, 5, 6,
				    1, 6, 2,
				    5, 4, 7,
				    5, 7, 6,
				    7, 3, 2,
				    7, 2, 6,
				    0, 5, 1,
				    0, 4, 5
  };

  float floor_vertices[] = {
			      -10.f, -10.f, -10.f,
			      10.f,  -10.f, -10.f,
			      -10.f, -10.f, 10.f,
			      10.f,  -10.f, 10.f
  };

  unsigned short floor_elements[] = {
					  2, 1, 0,
					  3, 1, 2
  };

  float rotation_matrix[16];
  mat4_create_rotation_x(rotation_matrix, 0.1f);

  World world;
  world_create(&world, gravity);

  Shape cube_shape;
  shape_create(&cube_shape, (Vector3*) cube_vertices, sizeof(cube_vertices) / (sizeof(Vector3)),
	       cube_elements, sizeof(cube_elements) / sizeof(unsigned short));

  Shape floor_shape;
  shape_create(&floor_shape, (Vector3*) floor_vertices, sizeof(floor_vertices) / (sizeof(Vector3)),
	       floor_elements, sizeof(floor_elements) / sizeof(unsigned short));

  /* shape_apply_transform(&floor_shape, rotation_matrix); */

  PhysicBody* cube_body = world_body_add(&world, &cube_shape, 1.f);
  PhysicBody* floor_body = world_body_add(&world, &floor_shape, 0.f);

  /* Creating a window and initialize an opengl context */
  GLFWwindow* window = opengl_window_create(800, 800, "Hello world");


  Drawable cube_drawable;
  Drawable floor_drawable;

  Vector3 cube_color[8];
  Vector3 floor_color[4];

  random_arrayf((float*)cube_color, 8 * 3);

  drawable_create(&cube_drawable, &cube_shape, cube_color);

  for (uint i = 0; i < 4; ++i) {
    floor_color[i].x = 1.f - ((float)i) / 4;
    floor_color[i].y = ((float)i) / 4;
    floor_color[i].z = 0.f;
  }

  drawable_create(&floor_drawable, &floor_shape, floor_color);

  /* Creating perspective matrix */
  float camera_perspective_matrix[16];
  mat4_create_perspective(camera_perspective_matrix, 1000.f, 0.1f);

  GLuint program_id = load_shaders("./shaders/vertex_shader_test.glsl", "./shaders/fragment_shader_test.glsl");
  GLuint matrix_id = glGetUniformLocation(program_id, "MVP");

  while(!glfwWindowShouldClose(window)) {
    clock_t time_start = clock();

    int width, height;
    float ratio;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the screen (color: white)

    /* Calculating the camera's forward vector */
    mat4_vector3_mul(&camera_direction, camera_orientation, camera_rotation_matrix);
    vector3_scalar_mul(&camera_direction, camera_direction, CAMERA_SPEED);

    /* Updating camera position */
    camera_create_rotation_matrix(camera_rotation_matrix, camera_rx, camera_ry);
    camera_create_final_matrix(camera_final_matrix, camera_perspective_matrix,
			       camera_rotation_matrix, camera_position);

    /* Updating the cube body */
    Vector3 cube_force;
    vector3_add(&cube_force, gravity, cube_direction);

    physics_body_apply_force(cube_body, cube_force);

    world_update(&world);

    drawable_update(&cube_drawable);
    drawable_update(&floor_drawable);

    cube_direction.x = 0.f;
    cube_direction.y = 0.f;
    cube_direction.z = 0.f;

    Collision cube_floor_collision = shape_shape_collide_convex(&floor_shape, &cube_shape);
    physics_body_solve_collision(floor_body, cube_body, cube_floor_collision);

    /* Drawing the scene */
    drawable_draw(&floor_drawable, program_id, camera_final_matrix, matrix_id);
    drawable_draw(&cube_drawable, program_id, camera_final_matrix, matrix_id);

    glfwSwapBuffers(window);
    glfwPollEvents();

    /* Keyboard input handling */
    if (glfwGetKey(window, GLFW_KEY_W))
      vector3_sub(&camera_position, camera_position, camera_direction);

    if (glfwGetKey(window, GLFW_KEY_S))
      vector3_add(&camera_position, camera_position, camera_direction);

    if (glfwGetKey(window, GLFW_KEY_J))
      cube_direction.x = 0.01f;

    if (glfwGetKey(window, GLFW_KEY_K))
      cube_direction.x = -0.01f;

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
    printf("Time elapsed : %ld\n", delta);
  }

  glfwTerminate(); // Exit the program
  return 0;
}
