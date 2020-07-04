#include <assert.h>
#include <float.h>

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

void vector3_neg(Vector3* dest) {
  dest->x = -dest->x;
  dest->y = -dest->y;
  dest->z = -dest->z;
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

float vector3_magnitude(Vector3 a) {
  return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

void vector3_normalize(Vector3* dest, Vector3 src) {
  float magnitude = vector3_magnitude(src);

  dest->x = src.x / magnitude;
  dest->y = src.y / magnitude;
  dest->z = src.z / magnitude;
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
	destination->D[i] += mat[j + i * 4];
      else
	destination->D[i] += v.D[j] * mat[j + i * 4];
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

  return vector3_dot(normal, relative_vector) <= 0.f;
}

void shape_update_normals(Shape* shape) {
  uint i = 0;
  for (i = 0; i < shape->indices_size; i += 3) {
    triangle_normal_from_vertices(shape->normals + i / 3,
				  shape->vertices[shape->indices[i]],
				  shape->vertices[shape->indices[i + 1]],
				  shape->vertices[shape->indices[i + 2]]);
  }
}

void shape_create(Shape* shape, Vector3* vertices, uint vertices_size,
		  unsigned short* indices, uint indices_size){
  shape->vertices = vertices;
  shape->vertices_size = vertices_size;

  shape->indices = indices;
  shape->indices_size = indices_size;

  shape->normals = malloc(sizeof(Vector3) * (indices_size / 3));

  shape_update_normals(shape);
}

void shape_destroy(Shape* shape) {
  free(shape->normals);

  shape->normals = NULL;
  shape->indices = NULL;
  shape->vertices = NULL;
}

bool shape_point_collide_convex(Shape* shape, Vector3 point) {
  for (uint i = 0; i < shape->indices_size; i += 3) {
    if (!triangle_point_collide(shape->normals[i / 3],
				shape->vertices[shape->indices[i]], point)) {
      return 0;
    }
  }

  return 1;
}

Collision shape_shape_collide_convex(Shape* shape1, Shape* shape2) {
  Collision result;

  result.shape_vertex = -1;
  result.vertex_id = -1;

  for (uint i = 0; i < shape1->vertices_size; ++i) {
    if (shape_point_collide_convex(shape2, shape1->vertices[i])) {
      result.vertex_id = i;
      result.shape_vertex = 0;

      return result;
    }
  }

  for (uint i = 0; i < shape2->vertices_size; ++i) {
    if (shape_point_collide_convex(shape1, shape2->vertices[i])) {
      result.vertex_id = i;
      result.shape_vertex = 1;

      return result;
    }
  }

  return result;
}

Vector3 shape_collision_normal(Shape* shape, Vector3 point) {
  float min_distance = FLT_MAX;
  uint min_normal_id;

  for (uint i = 0; i < shape->indices_size; i += 3) {
    Vector3 distance_vector;
    vector3_sub(&distance_vector, point,
		shape->vertices[shape->indices[i]]);

    float distance = vector3_magnitude(distance_vector);

    if (distance < min_distance) {
      min_distance = distance;
      min_normal_id = i / 3;
    }
  }

  return shape->normals[min_normal_id];
}

void shape_apply_transform(Shape* shape, Mat4 transform) {
  for (uint i = 0; i < shape->vertices_size; ++i) {
    mat4_vector3_mul(shape->vertices + i, shape->vertices[i], transform);
  }

  shape_update_normals(shape);
}
