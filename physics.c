#include <stdio.h>

#include "physics.h"

void physics_body_create(PhysicBody* body, Shape* shape, float mass) {
  body->shape = shape;

  if (mass == 0.f)
    body->mass_inv = 0.f;
  else
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

void physics_body_solve_collision(PhysicBody* body1, PhysicBody* body2, Collision collision) {
    if (collision.vertex_id == -1) {
      return;
    }

    Vector3 collision_normal;

    if (collision.shape_vertex == 0)
      collision_normal = shape_collision_normal(body2->shape,
						body1->shape->vertices[collision.vertex_id]);
    if (collision.shape_vertex == 1)
      collision_normal = shape_collision_normal(body1->shape,
						body2->shape->vertices[collision.vertex_id]);

    vector3_normalize(&collision_normal, collision_normal);

    Vector3 relative_velocity;
    vector3_sub(&relative_velocity, body1->velocity, body2->velocity);

    float vel_along_normal = vector3_dot(relative_velocity, collision_normal);

    if(vel_along_normal < 0)
      return;

    const float restitution = 1.f;
    const float j = (-(1 + restitution) * vel_along_normal) / (body1->mass_inv + body2->mass_inv);

    Vector3 impulse;
    vector3_scalar_mul(&impulse, collision_normal, j);

    Vector3 impulse_body1;
    Vector3 impulse_body2;
    vector3_scalar_mul(&impulse_body1, impulse, body1->mass_inv);
    vector3_scalar_mul(&impulse_body2, impulse, body2->mass_inv);

    vector3_add(&body1->velocity, body1->velocity, impulse_body1);
    vector3_sub(&body2->velocity, body2->velocity, impulse_body2);

    /* const float percent = 0.2; // usually 20% to 80% */
    /* Vector3 correction = (penetrationDepth / (A.inv_mass + B.inv_mass)) * percent * n; */

    /* A.position -= A.inv_mass * correction; */
    /* B.position += B.inv_mass * correction; */
}
