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

#ifndef __PHYSICS__
#define __PHYSICS__

#include <stdio.h>
#include <assert.h>
#include "Parameters.h"
#include "Particle.hpp"
#include "../util/Math_etc.h"

// Constants.
#define DEFAULT_RADIUS 0.5f
#define DEFAULT_MASS 1.0f
#define DEFAULT_CHARGE 0.0f
#define CHARGE_CONSTANT 1.0f
#define DEFAULT_BOND_STRENGTH 0.1f
#define MAX_BOND_LENGTH 5.0f
#define COEFFICIENT_OF_RESTITUTION 1.0f
#define MAX_VELOCITY 0.5f
#define VISCOSITY_FRICTION 0.1f
#define BROWNIAN_PROBABILITY 0.02f
#define MAX_BROWNIAN_FORCE 0.05f
#define MAX_PARTICLES 5000

// Quantized positioning.
#define POSITION(x) ((float)((int)(x)) + 0.5f)

class Physics
{
    public:

        // Particles.
        Particle *particles;
        int numParticles;

        // Constructor.
        Physics();

        // Destructor.
        ~Physics();

        // Create particle.
        Particle *createParticle(int type, float radius,
            float mass, float charge);
        Particle *createParticle(int type);

        // Add particle.
        void addParticle(Particle *particle);
        void addParticle(Particle *particle, Vector3D &velocity);

        // Remove particle.
        void removeParticle(Particle *particle);

        // Is particle in system?
        bool isValidParticle(Particle *particle);

        // Bond particles.
        bool createBond(Particle *particle1, int direction1,
            Particle *particle2, int direction2);
        bool createBond(Particle *particle1, int direction1,
            Particle *particle2, int direction2, float strength);

        // Remove bond.
        void removeBond(Particle *particle1, int direction);
        void removeBond(Particle *particle1, Particle *particle2);

        // Step system by given time increment.
        void step(float dtime);

        // Load and save particles.
        void load(FILE *fp);
        void save(FILE *fp);

    private:

        // Particle collisions.
        class Collision
        {
            public:

                Particle *particle1;
                Particle *particle2;
                Vector3D vCollisionNormal;
                Vector3D vCollisionPoint;
                Vector3D vRelativeVelocity;
                Collision *next;

                Collision()
                {
                    particle1 = particle2 = NULL;
                    next = NULL;
                }
        };
        Collision *collisions;

        // Check for particle collisions.
        void checkCollisions(Particle *particle);

        // Resolve collisions.
        void resolveCollisions();

        // Update charge forces.
        void updateChargeForces();

        // Update bond forces.
        void updateBondForces();
};
#endif
