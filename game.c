#include "render.h"
#include "misc.h"

#include <stdio.h>
#include <string.h>

#include <GLFW/glfw3.h>

#define MOUSE_SENSIBILLITY 0.01
#define CAMERA_SPEED 0.1f

extern float shape_cube_vertices[108];

double last_xpos = -1.0, last_ypos = -1.0;

float camera_perspective_matrix[16];
float camera_rotation_matrix[16];
float camera_final_matrix[16];
float rotation_matrix[16];

Vector4 camera_orientation = {0.f, 0.f, 1.f, 0.f};
Vector3 camera_position = {0.f, 0.3f, -4.f};
Vector3 camera_direction;
float camera_rx = 0.f, camera_ry = 0.f;

ConvexShape cube1_shape;
Vector3 cube1_shape_vertices[36];
Vector3 cube1_shape_color[36];
Drawable cube1_drawable;

ConvexShape cube2_shape;
Vector3 cube2_shape_vertices[36];
Vector3 cube2_shape_color[36];
Drawable cube2_drawable;

void game_init() {
  mat4_create_perspective(camera_perspective_matrix, 100.f, 0.1f); /* Creating perspective matrix */
  mat4_create_rotation_x(rotation_matrix, 0.01f);

  /* Cube color buffer and vertex buffer */
  random_arrayf((float*) cube1_shape_color, 108);
  memcpy(cube1_shape_vertices, shape_cube_vertices, sizeof(shape_cube_vertices));

  convex_shape_create(&cube1_shape, cube1_shape_vertices, 36);
  drawable_create(&cube1_drawable, &cube1_shape, cube1_shape_color);

  random_arrayf((float*) cube2_shape_color, 108);
  memcpy(cube2_shape_vertices, shape_cube_vertices, sizeof(shape_cube_vertices));

  convex_shape_create(&cube2_shape, cube2_shape_vertices, 36);
  drawable_create(&cube2_drawable, &cube2_shape, cube2_shape_color);
}

void game_loop(GLuint program_id, GLuint matrix_id){
    /* Updating camera position */
    camera_create_rotation_matrix(camera_rotation_matrix, camera_rx, camera_ry);
    camera_create_final_matrix(camera_final_matrix, camera_perspective_matrix,
			       camera_rotation_matrix, camera_position);

    /* Calculating the forward vector */
    Vector4 camera_direction_vec4;
    mat4_vector4_mul(&camera_direction_vec4, camera_orientation, camera_rotation_matrix);

    camera_direction = *((Vector3*) &camera_direction_vec4);
    vector3_scalar_mul(&camera_direction, camera_direction, CAMERA_SPEED);

    /* Updating and drawing the cube */
    drawable_update(&cube1_drawable, rotation_matrix);

    shape_draw(&cube1_drawable, program_id, camera_final_matrix, matrix_id);
    shape_draw(&cube2_drawable, program_id, camera_final_matrix, matrix_id);

    /* Testing collision */
    printf("Is colliding ? %d\n", convex_shape_shape_collide(&cube1_shape, &cube2_shape));
}

void game_handle_events(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_W)) {
    vector3_add(&camera_position, camera_position, camera_direction);

    for (uint i = 0; i < cube2_shape.vertices_size; ++i) {
      vector3_sub(cube2_shape.vertices + i, cube2_shape.vertices[i], camera_direction);
    }

    drawable_update(&cube2_drawable, NULL);
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    vector3_sub(&camera_position, camera_position, camera_direction);

    for (uint i = 0; i < cube2_shape.vertices_size; ++i) {
      vector3_add(cube2_shape.vertices + i, cube2_shape.vertices[i], camera_direction);
    }

    drawable_update(&cube2_drawable, NULL);
  }

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
