#ifndef LINEAR_ALGEBRA_HEADER
#define LINEAR_ALGEBRA_HEADER

typedef unsigned int uint;
typedef unsigned char bool;

typedef union
{
  struct {
    float x, y, z;
  };

  float D[3];
} Vector3;

typedef union
{
  struct {
    float x, y, z, w;
  };

  float D[4];
} Vector4;

/* Colums : i, rows : j */
typedef float* Mat4;

typedef struct
{
  Vector3* vertices;
  Vector3* normals;
  uint vertices_size;
} ConvexShape;

void vector3_add(Vector3* dest, Vector3 a, Vector3 b);
void vector3_sub(Vector3* dest, Vector3 a, Vector3 b);
void vector3_scalar_mul(Vector3* dest, Vector3 a, float s);
void vector3_apply_transform(Mat4 transform, Vector3* array, uint size);
float vector3_dot(Vector3 a, Vector3 b);

Mat4 mat4_allocate();
void mat4_create_translation(Mat4 destination, Vector3 direction);
void mat4_create_scale(Mat4 destination, Vector3 scale);
void mat4_create_perspective(Mat4 destination, float far, float near);

void mat4_create_rotation_x(Mat4 destination, float angle);
void mat4_create_rotation_y(Mat4 destination, float angle);
void mat4_create_rotation_z(Mat4 destination, float angle);

void mat4_mat4_mul(Mat4 destination, Mat4 a, Mat4 b);
void mat4_vector4_mul(Vector4* destination, Vector4 v, Mat4 mat);
void mat4_vector3_mul(Vector3* destination, Vector3 v, Mat4 mat);
void mat4_print(Mat4 m);

void triangle_normal_from_vertices(Vector3* n, Vector3 A, Vector3 B, Vector3 C);
bool triangle_point_collide(Vector3 normal, Vector3 point, Vector3 p);

void convex_shape_create(ConvexShape* shape, Vector3* vertices, uint vertices_size);
bool convex_shape_point_collide(ConvexShape* shape, Vector3 point);

#endif
