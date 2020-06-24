#include "render.h"
#include "misc.h"

#include <stdio.h>
#include <string.h>

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

extern float shape_cube_vertices[108];

ConvexShape cube_shape;

Vector3 cube_shape_vertices[36];
Vector3 cube_shape_color[36];

GLuint cube_shape_vertex_buffer, cube_shape_fragment_buffer;

float cube_rotation_matrix[16];

void game_init() {
  mat4_create_perspective(camera_perspective_matrix, 100.f, 0.1f); /* Creating perspective matrix */
  mat4_create_rotation_x(cube_rotation_matrix, 0.01f);

  /* Cube color buffer and vertex buffer */
  random_arrayf((float*) cube_shape_color, 108);
  memcpy(cube_shape_vertices, shape_cube_vertices, sizeof(shape_cube_vertices));

  convex_shape_create(&cube_shape, cube_shape_vertices, 36);

  cube_shape_vertex_buffer = create_array_buffer(sizeof(cube_shape_vertices));
  cube_shape_fragment_buffer = create_array_buffer(sizeof(cube_shape_color));

  update_array_buffer(cube_shape_fragment_buffer, (float*) cube_shape_color, sizeof(cube_shape_color));
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

    vector3_apply_transform(cube_rotation_matrix, cube_shape.vertices, 36);
    update_array_buffer(cube_shape_vertex_buffer, (float*) cube_shape.vertices, sizeof(shape_cube_vertices));
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
