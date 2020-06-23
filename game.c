#include "render.h"

#include <stdio.h>

#include <GLFW/glfw3.h>

#define MOUSE_SENSIBILLITY 0.01
#define CAMERA_SPEED 0.1f

double last_xpos = -1.0, last_ypos = -1.0;

float camera_perspective_matrix[16];
float camera_rotation_matrix[16];
float camera_final_matrix[16];

Vector3 camera_position = {0.f, 0.3f, -4.f};
float camera_rx = 0.f, camera_ry = 0.f;

Vector4 camera_orientation = {0.f, 0.f, 1.f, 0.f};
Vector3 camera_direction;

void game_init() {
  mat4_create_perspective(camera_perspective_matrix, 100.f, 0.1f);
}

void game_loop() {
    /* Updating camera position */
    camera_create_rotation_matrix(camera_rotation_matrix, camera_rx, camera_ry);
    camera_create_final_matrix(camera_final_matrix, camera_perspective_matrix,
			       camera_rotation_matrix, camera_position);

    /* Calculating the forward vector */
    Vector4 camera_direction_vec4;
    mat4_vector4_mul(&camera_direction_vec4, camera_orientation, camera_rotation_matrix);

    camera_direction = *((Vector3*) &camera_direction_vec4);

    vector3_scalar_mul(&camera_direction, camera_direction, CAMERA_SPEED);
}

void game_handle_events(GLFWwindow* window) {
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
