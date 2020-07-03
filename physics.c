#include <stdio.h>

#include "physics.h"

void physics_body_create(PhysicBody* body, Shape* shape, float mass) {
  body->shape = shape;
  body->mass_inv = 1.f / mass;

  body->velocity.x = 0.f;
  body->velocity.y = 0.f;
  body->velocity.z = 0.f;
}

void physics_body_update(PhysicBody* body, Vector3 force) {
  vector3_scalar_mul(&force, force, body->mass_inv); /* Adding force to velocity */
  vector3_add(&body->velocity, body->velocity, force);

  /* Applying velocity to the whole shape */
  for (uint i = 0; i < body->shape->vertices_size; ++i)
    vector3_add(body->shape->vertices + i, body->shape->vertices[i], body->velocity);
}
