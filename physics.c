#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "physics.h"

#define DEFAULT_WORLD_CAPACITY 10

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

void physics_body_apply_force(PhysicBody* body, Vector3 force) {
  vector3_scalar_mul(&force, force, body->mass_inv); /* Adding force to velocity */
  vector3_add(&body->velocity, body->velocity, force);
}

void physics_body_update(PhysicBody* body) {
  /* Applying velocity to the whole shape */
  for (uint i = 0; i < body->shape->vertices_size; ++i)
    vector3_add(body->shape->vertices + i, body->shape->vertices[i], body->velocity);
}

void physics_body_solve_collision(PhysicBody* body1, PhysicBody* body2, Collision collision) {
  /* Solve only if colliding */
  if (collision.vertex_id == -1) {
    return;
  }

  uint collision_normal_id;
  Vector3 collision_normal;
  float penetration;

  /* Calculating the closest normal and the penetration depth */
  if (collision.shape_vertex == 0) {
    collision_normal_id =
      shape_collision_normal(body2->shape, body1->shape->vertices[collision.vertex_id]);

    collision_normal = body2->shape->normals[collision_normal_id];
    vector3_normalize(&collision_normal, collision_normal);

    penetration = triangle_point_collide(collision_normal,
					 body2->shape->vertices[body2->shape->indices[collision_normal_id / 3]],
					 body1->shape->vertices[collision.vertex_id]);
  }

  if (collision.shape_vertex == 1) {
    collision_normal_id =
      shape_collision_normal(body1->shape, body2->shape->vertices[collision.vertex_id]);

    collision_normal = body1->shape->normals[collision_normal_id];
    vector3_normalize(&collision_normal, collision_normal);

    penetration = triangle_point_collide(collision_normal,
					 body1->shape->vertices[body1->shape->indices[collision_normal_id / 3]],
					 body2->shape->vertices[collision.vertex_id]);
  }


  /* Computing the relative velocity and the velocity along the normal */
  Vector3 relative_velocity;
  vector3_sub(&relative_velocity, body1->velocity, body2->velocity);

  float vel_along_normal = vector3_dot(relative_velocity, collision_normal);

  /* If they won't intersect, don't slove */
  if(vel_along_normal < 0)
    return;

  const float restitution = 0.5f;
  const float j = (-(1 + restitution) * vel_along_normal) / (body1->mass_inv + body2->mass_inv);

  Vector3 impulse;
  vector3_scalar_mul(&impulse, collision_normal, j);

  Vector3 impulse_body1;
  Vector3 impulse_body2;
  vector3_scalar_mul(&impulse_body1, impulse, body1->mass_inv);
  vector3_scalar_mul(&impulse_body2, impulse, body2->mass_inv);

  /* Applying the impulses */
  vector3_add(&body1->velocity, body1->velocity, impulse_body1);
  vector3_sub(&body2->velocity, body2->velocity, impulse_body2);

  /* Position correction */
  const float percent = 0.8; // usually 20% to 80%
  const float correction = (penetration / (body1->mass_inv + body2->mass_inv)) * percent;

  Vector3 correction_body1;
  Vector3 correction_body2;

  vector3_scalar_mul(&correction_body1, collision_normal,  correction * body1->mass_inv);
  vector3_scalar_mul(&correction_body2, collision_normal, -correction * body2->mass_inv);

  for (uint i = 0; i < body1->shape->vertices_size; ++i)
    vector3_add(body1->shape->vertices + i, body1->shape->vertices[i], correction_body1);

  for (uint i = 0; i < body2->shape->vertices_size; ++i)
    vector3_add(body2->shape->vertices + i, body2->shape->vertices[i], correction_body2);
}

void world_create(World* world, Vector3 gravity) {
  world->bodies_number = 0;
  world->gravity = gravity;

  world->bodies_capacity = DEFAULT_WORLD_CAPACITY;

  world->bodies = malloc(DEFAULT_WORLD_CAPACITY * sizeof(PhysicBody));
}

PhysicBody* world_body_add(World *world, Shape* shape, float mass) {
  if(world->bodies_number >= world->bodies_capacity) {
    world->bodies_capacity += DEFAULT_WORLD_CAPACITY;

    world->bodies = realloc(world->bodies, world->bodies_capacity * sizeof(PhysicBody));
  }

  PhysicBody* new_body = world->bodies + world->bodies_number;

  physics_body_create(new_body, shape, mass);
  world->bodies_number++;

  return new_body;
}

void world_update(World *world) {
  for (uint i = 0; i < world->bodies_number; ++i) {
    physics_body_update(world->bodies + i);
  }
}
