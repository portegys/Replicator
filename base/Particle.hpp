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
 * Particle.
 * Source: "Physics for Game Developers", Copyright 2000-2001 by David Bourg.
 */

#ifndef __PARTICLE__
#define __PARTICLE__

#include <stdio.h>
#include "../util/Math_etc.h"
#include "Orientation.hpp"
#include "Bond.hpp"

class Particle
{
    public:

        int id;                                   // id
        int type;                                 // type
        int state;                                // state
        float fRadius;                            // radius
        float fMass;                              // mass
        Matrix3x3 mInertia;                       // mass moment of inertia
        Matrix3x3 mInertiaInverse;                // inverse of mass moment of inertia
        float fCharge;                            // charge
        float coefficientOfRestitution;
        Orientation orientation;                  // orientation
        Particle *bonds[8];                       // bonds to particles
        Bond *bondProperties[8];                  // bond properties
        Vector3D vPosition;                       // position
        Vector3D vVelocity;                       // velocity
        Vector3D vForces;                         // force
        Particle *collide;
        Particle *next;

        // Constructor.
        Particle(int type, float radius, float mass, float charge);
        Particle(int type);

        // Destructor.
        ~Particle();

        // Duplicate particle.
        Particle *duplicate();

        // Calculate inertia.
        void calcInertia();

        // Read and write particle.
        static Particle *read(FILE *fp);
        static void write(FILE *fp, Particle *particle);

        // ID factory.
        static int idFactory;
};
#endif
