#include "linear_algebra.h"

typedef struct {
  Shape* shape;
  float mass_inv;

  Vector3 velocity;
} PhysicBody;

void physics_body_create(PhysicBody* body, Shape* shape, float mass);
void physics_body_update(PhysicBody* body, Vector3 force);
void physics_body_solve_collision(PhysicBody* body1, PhysicBody* body2, Collision collision);
