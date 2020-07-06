#include "linear_algebra.h"

typedef struct {
  Shape* shape;
  float mass_inv;

  Vector3 velocity;
} PhysicBody;

typedef struct {
  Vector3 gravity;

  PhysicBody* bodies;

  uint bodies_number;
  uint bodies_capacity;
} World;

void world_create(World* world, Vector3 gravity);
PhysicBody* world_body_add(World *world, Shape* shape, float mass);
void world_update(World *world);

void physics_body_update(PhysicBody* body);
void physics_body_apply_force(PhysicBody* body, Vector3 force);
void physics_body_solve_collision(PhysicBody* body1, PhysicBody* body2, Collision collision);
