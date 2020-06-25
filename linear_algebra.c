#include <assert.h>

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "linear_algebra.h"

void vector3_add(Vector3* dest, Vector3 a, Vector3 b) {
  dest->x = a.x + b.x;
  dest->y = a.y + b.y;
  dest->z = a.z + b.z;
}

void vector3_sub(Vector3* dest, Vector3 a, Vector3 b) {
  dest->x = a.x - b.x;
  dest->y = a.y - b.y;
  dest->z = a.z - b.z;
}

void vector3_scalar_mul(Vector3* dest, Vector3 a, float s) {
  dest->x = a.x * s;
  dest->y = a.y * s;
  dest->z = a.z * s;
}

void vector3_cross(Vector3* dest, Vector3 a, Vector3 b) {
  dest->x = a.y * b.z - a.z * b.y;
  dest->y = a.z * b.x - a.x * b.z;
  dest->z = a.x * b.y - a.y * b.x;
}

float vector3_dot(Vector3 a, Vector3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Mat4 mat4_allocate() {
  Mat4 mat = malloc(sizeof(float) * 16);
  return mat;
}

void mat4_create_translation(Mat4 destination, Vector3 direction) {
  for (uint i = 0; i < 4; ++i) {
    for (uint j = 0; j < 4; ++j) {
      if (i == 3 && j != 3)
	destination[j + i * 4] = direction.D[j];
      else if (i == j)
	destination[j + i * 4] = 1;
      else
	destination[j + i * 4] = 0;
    }
  }
}

void mat4_create_scale(Mat4 destination, Vector3 scale) {
  for (uint i = 0; i < 4; ++i) {
    for (uint j = 0; j < 4; ++j) {
      if (i == j && j < 3)
	destination[j + i * 4] = scale.D[j];
      else if (i == j)
	destination[j + i * 4] = 1;
      else
	destination[j + i * 4] = 0;
    }
  }
}

void mat4_create_perspective(Mat4 destination, float far, float near){
  for (uint i = 0; i < 16; ++i) {
    destination[i] = 0;
  }

  destination[0] = 1;
  destination[5] = 1;

  destination[10] = -(far) / (far - near);
  destination[11] = -1;
  destination[14] = -(far * near) / (far - near);
}

void mat4_create_rotation_x(Mat4 destination, float angle){
  for (uint i = 0; i < 4; ++i) {
    for (uint j = 0; j < 4; ++j) {
      if (i == j)
	destination[j + i * 4] = 1;
      else
	destination[j + i * 4] = 0;
    }
  }

  destination[5] = cosf(angle);
  destination[6] = sinf(angle);

  destination[9] = -sinf(angle);
  destination[10] = cosf(angle);
}

void mat4_create_rotation_y(Mat4 destination, float angle){
  for (uint i = 0; i < 4; ++i) {
    for (uint j = 0; j < 4; ++j) {
      if (i == j)
	destination[j + i * 4] = 1;
      else
	destination[j + i * 4] = 0;
    }
  }

  destination[0] = cosf(angle);
  destination[2] = sinf(angle);

  destination[8] = -sinf(angle);
  destination[10] = cosf(angle);
}

void mat4_create_rotation_z(Mat4 destination, float angle){
  for (uint i = 0; i < 4; ++i) {
    for (uint j = 0; j < 4; ++j) {
      if (i == j)
	destination[j + i * 4] = 1;
      else
	destination[j + i * 4] = 0;
    }
  }

  destination[0] = cosf(angle);
  destination[1] = sinf(angle);

  destination[4] = -sinf(angle);
  destination[5] = cosf(angle);
}

void mat4_vector4_mul(Vector4* destination, Vector4 v, Mat4 mat) {
  for (uint i = 0; i < 4; ++i) {
    destination->D[i] = 0.f;

    for (uint j = 0; j < 4; ++j) {
      destination->D[i] += v.D[j] * mat[j + i * 4];
    }
  }
}

void mat4_vector3_mul(Vector3* destination, Vector3 v, Mat4 mat) {
  for (uint i = 0; i < 3; ++i) {
    destination->D[i] = 0.f;

    for (uint j = 0; j < 4; ++j) {
      if(j == 3)
	destination->D[i] += mat[j * 4 + i];
      else
	destination->D[i] += v.D[j] * mat[j * 4 + i];
    }
  }
}

void mat4_mat4_mul(Mat4 destination, Mat4 a, Mat4 b) {
  for (uint i = 0; i < 4; ++i) {
    for (uint j = 0; j < 4; ++j) {
      destination[j + i * 4] = 0.f;

      for (uint k = 0; k < 4; ++k) {
	destination[j + i * 4] += a[i * 4 + k] * b[j + k * 4];
      }
    }
  }
}

void mat4_print(Mat4 m) {
  for (uint i = 0; i < 4; ++i) {
    for (uint j = 0; j < 4; ++j) {
      printf("%.2f\t", m[j + i * 4]);
    }

    printf("\n");
  }
}

void triangle_normal_from_vertices(Vector3* n, Vector3 A, Vector3 B, Vector3 C) {
  Vector3 triangle_edge1;
  Vector3 triangle_edge2;

  vector3_sub(&triangle_edge1, B, A);
  vector3_sub(&triangle_edge2, C, A);

  vector3_cross(n, triangle_edge1, triangle_edge2);
}

bool triangle_point_collide(Vector3 normal, Vector3 point, Vector3 p) {
  Vector3 relative_vector;
  vector3_sub(&relative_vector, p, point);

  return vector3_dot(normal, relative_vector) >= 0.f;
}

void convex_shape_create(ConvexShape* shape, Vector3* vertices, uint vertices_size){
  assert(vertices_size % 3 == 0); /* Make sure that this is an array of triangles */

  shape->vertices = vertices;
  shape->vertices_size = vertices_size;

  shape->normals = malloc(sizeof(Vector3) * (vertices_size / 3));

  uint i = 0;
  for (i = 0; i < vertices_size; i += 3) {
    triangle_normal_from_vertices(shape->normals + i / 3,
				  vertices[i], vertices[i + 1], vertices[i + 2]);
  }
}

bool convex_shape_point_collide(ConvexShape* shape, Vector3 point) {
  for (uint i = 0; i < shape->vertices_size; ++i) {
    if (!triangle_point_collide(shape->normals[i / 3], shape->vertices[i], point)) {
      return 0;
    }
  }

  return 1;
}

void convex_shape_apply_transform(ConvexShape* shape, Mat4 transform) {
  for (uint i = 0; i < shape->vertices_size; ++i) {
    mat4_vector3_mul(shape->vertices + i, shape->vertices[i], transform);
  }
}
