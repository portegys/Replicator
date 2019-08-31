/*
 * This software is provided under the terms of the GNU General
 * Public License as published by the Free Software Foundation.
 *
 * Copyright (c) 2004 Tom Portegys, All Rights Reserved.
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.
 */

/*
 * Physics for colliding bodies connected by bonds and charge forces.
 * Source: "Physics for Game Developers", Copyright 2000-2001 by David Bourg.
 */

#include <stdlib.h>
#include "Physics.hpp"
#include "../util/Random.hpp"

// Constructor.
Physics::Physics()
{
    particles = NULL;
    numParticles = 0;
    collisions = NULL;
}


// Destructor.
Physics::~Physics()
{
    Particle *particle;

    while (particles != NULL)
    {
        particle = particles->next;
        delete particles;
        particles = particle;
    }
}


// Create particle.
Particle *Physics::createParticle(int type, float radius,
float mass, float charge)
{
    if (numParticles >= MAX_PARTICLES) return NULL;
    Particle *particle = new Particle(type, radius,
        mass, charge);
    assert(particle != NULL);
    addParticle(particle);
    return(particle);
}


Particle *Physics::createParticle(int type)
{
    if (numParticles >= MAX_PARTICLES) return NULL;
    Particle *particle = new Particle(type);
    assert(particle != NULL);
    addParticle(particle);
    return(particle);
}


// Add particle to system.
void Physics::addParticle(Particle *particle)
{
    Vector3D velocity;
    addParticle(particle, velocity);
}


void Physics::addParticle(Particle *particle, Vector3D &velocity)
{
    particle->vVelocity = velocity;
    particle->next = particles;
    particles = particle;
    numParticles++;
}


// Remove particle from system.
void Physics::removeParticle(Particle *particle)
{
    Particle *particle2,*particle3;

    for (particle2 = particles, particle3 = NULL;
        particle2 != NULL && particle2 != particle;
        particle3 = particle2, particle2 = particle2->next) {}
    if (particle2 == NULL) return;
    if (particle3 == NULL)
    {
        particles = particle2->next;
    }
    else
    {
        particle3->next = particle2->next;
    }
    delete particle;
    numParticles--;
}


// Is particle valid?
bool Physics::isValidParticle(Particle *particle)
{
    Particle *particle2;

    for (particle2 = particles; particle2 != NULL;
        particle2 = particle2->next)
    {
        if (particle2 == particle) return true;
    }
    return false;
}


// Bond particles.
bool Physics::createBond(Particle *particle1, int direction1,
Particle *particle2, int direction2)
{
    return createBond(particle1, direction1,
        particle2, direction2, DEFAULT_BOND_STRENGTH);
}


bool Physics::createBond(Particle *particle1, int direction1,
Particle *particle2, int direction2, float strength)
{
    if (particle1 == particle2) return false;
    if (particle1->bonds[direction1] == particle2 &&
        particle2->bonds[direction2] == particle1)
    {
        return true;
    }
    if (particle1->bonds[direction1] != NULL ||
        particle2->bonds[direction2] != NULL)
    {
        return false;
    }
    particle1->bonds[direction1] = particle2;
    particle2->bonds[direction2] = particle1;
    particle1->bondProperties[direction1] = new Bond(strength);
    assert(particle1->bondProperties[direction1] != NULL);
    particle2->bondProperties[direction2] =
        particle1->bondProperties[direction1];
    return true;
}


// Remove bond.
void Physics::removeBond(Particle *particle1, int direction1)
{
    Particle *particle2;

    if ((particle2 = particle1->bonds[direction1]) != NULL)
    {
        for (int i = 0; i < 8; i++)
        {
            if (particle2->bonds[i] == particle1)
            {
                particle2->bonds[i] = NULL;
                particle2->bondProperties[i] = NULL;
                break;
            }
        }
        particle1->bonds[direction1] = NULL;
        delete particle1->bondProperties[direction1];
        particle1->bondProperties[direction1] = NULL;
    }
}


void Physics::removeBond(Particle *particle1, Particle *particle2)
{
    for (int i = 0; i < 8; i++)
    {
        if (particle1->bonds[i] == particle2)
        {
            particle1->bonds[i] = NULL;
            delete particle1->bondProperties[i];
            particle1->bondProperties[i] = NULL;
        }
        if (particle2->bonds[i] == particle1)
        {
            particle2->bonds[i] = NULL;
            particle2->bondProperties[i] = NULL;
        }
    }
}


// Step system by given time increment.
void Physics::step(float dtime)
{
    Particle *particle,*particle2;
    Collision *collision;
    float dist;
    int i,j;

    // Integrate.
    for (particle = particles; particle != NULL; particle = particle->next)
    {
        // Add Brownian motion force.
        if (Random::nextDouble() < BROWNIAN_PROBABILITY)
        {
            if (Random::nextBoolean())
            {
                particle->vForces.x += Random::nextFloat() * MAX_BROWNIAN_FORCE;
            }
            else
            {
                particle->vForces.x -= Random::nextFloat() * MAX_BROWNIAN_FORCE;
            }
            if (Random::nextBoolean())
            {
                particle->vForces.y += Random::nextFloat() * MAX_BROWNIAN_FORCE;
            }
            else
            {
                particle->vForces.y -= Random::nextFloat() * MAX_BROWNIAN_FORCE;
            }
        }

        // Update the velocity of the particle due to forces.
        particle->vVelocity += (particle->vForces / particle->fMass) * dtime;
        if (particle->vVelocity.Magnitude() > MAX_VELOCITY)
        {
            particle->vVelocity.Normalize(MAX_VELOCITY);
        }

        // Apply viscosity friction.
        particle->vVelocity *= (1.0f - VISCOSITY_FRICTION);

        // Update the position of the particle.
        particle->vPosition += particle->vVelocity * dtime;
        if (particle->vPosition.x < POSITION(0.0f))
        {
            particle->vPosition.x = POSITION(0.0f);
        }
        if (particle->vPosition.x > POSITION(WIDTH - 1))
        {
            particle->vPosition.x = POSITION(WIDTH - 1);
        }
        if (particle->vPosition.y < POSITION(0.0f))
        {
            particle->vPosition.y = POSITION(0.0f);
        }
        if (particle->vPosition.y > POSITION(HEIGHT - 1))
        {
            particle->vPosition.y = POSITION(HEIGHT - 1);
        }

        // Reset forces.
        particle->vForces.Zero();
    }

    // Break overstretched bonds.
    for (particle = particles; particle != NULL;
        particle = particle->next)
    {
        for (i = 0; i < 8; i++)
        {
            if ((particle2 = particle->bonds[i]) == NULL) continue;
            dist = (particle->vPosition - particle2->vPosition).Magnitude();
            if (dist > MAX_BOND_LENGTH)
            {
                particle->bonds[i] = NULL;
                for (j = 0; j < 8; j++)
                {
                    if (particle2->bonds[j] == particle)
                    {
                        particle2->bonds[j] = NULL;
                    }
                }
            }
        }
    }

    // Update charge forces.
    updateChargeForces();

    // Update bond forces.
    updateBondForces();

    // Detect collisions.
    for (particle = particles; particle != NULL;
        particle = particle->next)
    {
        particle->collide = NULL;
    }
    for (particle = particles; particle != NULL;
        particle = particle->next)
    {
        checkCollisions(particle);
    }

    // Resolve collisions.
    resolveCollisions();

    // Release collisions.
    while (collisions != NULL)
    {
        collision = collisions;
        collisions = collision->next;
        delete collision;
    }
}


// Update charge forces.
void Physics::updateChargeForces()
{
    Particle *particle1,*particle2;
    Vector3D vForce;
    float dist;
    float s;

    for (particle1 = particles; particle1 != NULL;
        particle1 = particle1->next)
    {
        for (particle2 = particles; particle2 != NULL;
            particle2 = particle2->next)
        {
            if (particle1 == particle2) continue;
            vForce = particle1->vPosition - particle2->vPosition;
            dist = vForce.Magnitude();
            if (dist > 0.0f)
            {
                // Force is proportional to inverse square of distance.
                vForce.Normalize();
                s = (CHARGE_CONSTANT *
                    particle1->fCharge * particle2->fCharge) /
                    (dist * dist);
                vForce *= s;
                particle1->vForces += vForce;
            }
        }
    }
}


// Update bond forces on other particles.
// Bond force acts to move bonded particles to
// their proper relative positions according to their
// bonding orientations.
void Physics::updateBondForces()
{
    int i;
    Particle *particle1,*particle2;
    Vector3D vPosition,vForce;

    for (particle1 = particles; particle1 != NULL;
        particle1 = particle1->next)
    {
        for (i = 0; i < 8; i++)
        {
            if ((particle2 = particle1->bonds[i]) == NULL) continue;

            // Force on particle is proportional to distance
            // of particle from expected position.
            vPosition = particle1->vPosition;
            switch(i)
            {
                case NORTH:
                    vPosition.y += 1.0f;
                    break;
                case NORTHEAST:
                    vPosition.x += 1.0f;
                    vPosition.y += 1.0f;
                    break;
                case EAST:
                    vPosition.x += 1.0f;
                    break;
                case SOUTHEAST:
                    vPosition.x += 1.0f;
                    vPosition.y -= 1.0f;
                    break;
                case SOUTH:
                    vPosition.y -= 1.0f;
                    break;
                case SOUTHWEST:
                    vPosition.x -= 1.0f;
                    vPosition.y -= 1.0f;
                    break;
                case WEST:
                    vPosition.x -= 1.0f;
                    break;
                case NORTHWEST:
                    vPosition.x -= 1.0f;
                    vPosition.y += 1.0f;
                    break;
            }
            vForce = vPosition - particle2->vPosition;
            if (vForce.Magnitude() > 0.0f)
            {
                vForce *= particle1->bondProperties[i]->getStrength();
                particle2->vForces += vForce;
            }
        }
    }
}


// Check for collisions with body's particles.
void Physics::checkCollisions(Particle *particle1)
{
    Particle *particle2;
    Vector3D vnormal,vrelative;
    Collision *collision;

    if (particle1->collide != NULL) return;

    for (particle2 = particles;
        particle2 != NULL; particle2 = particle2->next)
    {
        if (particle1 == particle2) continue;
        if (particle2->collide != NULL) continue;

        // Particles intersect?
        vnormal = particle1->vPosition - particle2->vPosition;
        if (vnormal.Magnitude() < (particle1->fRadius + particle2->fRadius))
        {
            // Particles moving toward each other?
            vnormal.Normalize();
            vrelative = particle1->vVelocity - particle2->vVelocity;
            if ((vrelative * vnormal) < 0.0)
            {
                collision = new Collision();
                assert(collision != NULL);
                collision->particle1 = particle1;
                collision->particle2 = particle2;
                particle1->collide = particle2;
                particle2->collide = particle1;
                collision->vCollisionNormal = vnormal;
                collision->vCollisionPoint = (vnormal * particle1->fRadius) +
                    particle1->vPosition;
                collision->vRelativeVelocity = vrelative;
                collision->next = collisions;
                collisions = collision;
                return;
            }
        }
    }
}


// Resolve collisions.
void Physics::resolveCollisions()
{
    Collision *collision;
    Particle *particle1,*particle2;
    Vector3D pt1,pt2;
    float impulse;
    float coefficientOfRestitution;

    for (collision = collisions; collision != NULL; collision = collision->next)
    {
        particle1 = collision->particle1;
        particle2 = collision->particle2;

        // Calculate impulse force.
        pt1 = collision->vCollisionPoint - particle1->vPosition;
        pt2 = collision->vCollisionPoint - particle2->vPosition;
        coefficientOfRestitution = (particle1->coefficientOfRestitution +
            particle2->coefficientOfRestitution) / 2.0f;
        impulse =
            (-(1.0f + coefficientOfRestitution) *
            (collision->vRelativeVelocity * collision->vCollisionNormal)) /
            ((1.0f / particle1->fMass + 1.0f / particle2->fMass) +
            (collision->vCollisionNormal *
            (((pt1 ^ collision->vCollisionNormal) * particle1->mInertiaInverse) ^ pt1)) +
            (collision->vCollisionNormal *
            (((pt2 ^ collision->vCollisionNormal) * particle2->mInertiaInverse) ^ pt2))
            );

        // Accumulate forces.
        particle1->vForces += impulse * collision->vCollisionNormal;
        particle2->vForces -= impulse * collision->vCollisionNormal;
    }
}


// Load particles.
void Physics::load(FILE *fp)
{
    int i,j,id1,id2,num;
    Particle *particle1,*particle2;
    char buf[50];

    // Read particles.
    fscanf(fp, "%d", &num);
    for (i = 0; i < num; i++)
    {
        particle1 = Particle::read(fp);
        addParticle(particle1);
    }

    // Read bonds.
    for (i = 0; i < num; i++)
    {
        fscanf(fp, "%d", &id1);
        for (particle1 = particles; particle1 != NULL;
            particle1 = particle1->next)
        {
            if (particle1->id == id1) break;
        }
        for (j = 0; j < 8; j++)
        {
            fscanf(fp, "%d %s", &id2, buf);
            if (id2 == -1) continue;
            for (particle2 = particles; particle2 != NULL;
                particle2 = particle2->next)
            {
                if (particle2->id == id2) break;
            }
            particle1->bonds[j] = particle2;
            particle1->bondProperties[j] =
                new Bond((float)atof(buf));
        }
    }

    // Consolidate bonds.
    for (particle1 = particles; particle1 != NULL;
        particle1 = particle1->next)
    {
        for (i = 0; i < 8; i++)
        {
            if ((particle2 = particle1->bonds[i]) == NULL) continue;
            for (j = 0; j < 8; j++)
            {
                if (particle2->bonds[j] == particle1)
                {
                    if (particle1->bondProperties[i] != particle2->bondProperties[j])
                    {
                        delete particle2->bondProperties[j];
                        particle2->bondProperties[j] = particle1->bondProperties[i];
                    }
                    break;
                }
            }
        }
    }
}


// Save particles.
void Physics::save(FILE *fp)
{
    int i;
    Particle *particle;

    // Write particles.
    for (particle = particles, i = 0; particle != NULL;
        particle = particle->next, i++) {}
    fprintf(fp, "%d\n", i);
    for (particle = particles; particle != NULL;
        particle = particle->next)
    {
        Particle::write(fp, particle);
        fprintf(fp, "\n");
    }

    // Write bonds.
    for (particle = particles; particle != NULL;
        particle = particle->next)
    {
        fprintf(fp, "%d ", particle->id);
        for (i = 0; i < 8; i++)
        {
            if (particle->bonds[i] == NULL)
            {
                fprintf(fp, "-1 %f ", DEFAULT_BOND_STRENGTH);
            }
            else
            {
                fprintf(fp, "%d %f ", particle->bonds[i]->id,
                    particle->bondProperties[i]->getStrength());
            }
        }
        fprintf(fp, "\n");
    }
    fflush(fp);
}
