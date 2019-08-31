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
 * Body particle.
 * Source: "Physics for Game Developers", Copyright 2000-2001 by David Bourg.
 */

#include <stdlib.h>
#include <assert.h>
#include "Particle.hpp"
#include "Physics.hpp"

// ID factory.
int Particle::idFactory = 0;

// Constructor.
Particle::Particle(int type, float radius,
float mass, float charge)
{
    id = idFactory;
    idFactory++;
    this->type = type;
    state = 0;
    fRadius = radius;
    fMass = mass;
    calcInertia();
    fCharge = charge;
    coefficientOfRestitution = COEFFICIENT_OF_RESTITUTION;
    for (int i = 0; i < 8; i++)
    {
        bonds[i] = NULL;
        bondProperties[i] = NULL;
    }
    next = NULL;
}


Particle::Particle(int type)
{
    id = idFactory;
    idFactory++;
    this->type = type;
    state = 0;
    fRadius = DEFAULT_RADIUS;
    fMass = DEFAULT_MASS;
    calcInertia();
    fCharge = DEFAULT_CHARGE;
    coefficientOfRestitution = COEFFICIENT_OF_RESTITUTION;
    for (int i = 0; i < 8; i++)
    {
        bonds[i] = NULL;
        bondProperties[i] = NULL;
    }
    next = NULL;
}


// Destructor.
Particle::~Particle()
{
    Particle *particle;
    for (int i = 0; i < 8; i++)
    {
        if ((particle = bonds[i]) != NULL)
        {
            for (int j = 0; j < 8; j++)
            {
                if (particle->bonds[j] == this)
                {
                    particle->bonds[j] = NULL;
                    delete particle->bondProperties[j];
                    particle->bondProperties[j] = NULL;
                }
            }
        }
    }
}


// Duplicate particle.
Particle *Particle::duplicate()
{
    Particle *particle = new Particle(type, fRadius,
        fMass, fCharge);
    assert(particle != NULL);
    particle->state = state;
    particle->coefficientOfRestitution = coefficientOfRestitution;
    particle->orientation.direction = orientation.direction;
    particle->orientation.mirrored = orientation.mirrored;
    for (int i = 0; i < 8; i++)
    {
        particle->bonds[i] = bonds[i];
        particle->bondProperties[i] = bondProperties[i];
    }
    particle->vPosition = vPosition;
    particle->vVelocity = vVelocity;
    particle->vForces = vForces;
    return(particle);
}


// Calculate inertia.
void Particle::calcInertia()
{
    float d = 2.0f * ((fRadius * 2.0f) * (fRadius * 2.0f));
    d = fMass/12.0f * d;
    mInertia.e11 = d; mInertia.e12 = 0; mInertia.e13 = 0;
    mInertia.e21 = 0; mInertia.e22 = d; mInertia.e23 = 0;
    mInertia.e31 = 0; mInertia.e32 = 0; mInertia.e33 = d;
    mInertiaInverse = mInertia.Inverse();
}


// Read particle.
Particle *Particle::read(FILE *fp)
{
    int value;
    char buf[50];
    Particle *particle = new Particle(0);
    assert(particle != NULL);
    fscanf(fp, "%d", &particle->id);
    if (idFactory <= particle->id)
    {
        idFactory = particle->id + 1;
    }
    fscanf(fp, "%d", &particle->type);
    fscanf(fp, "%d", &particle->state);
    fscanf(fp, "%s", buf);
    particle->fRadius = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->fMass = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->fCharge = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->coefficientOfRestitution = (float)atof(buf);
    fscanf(fp, "%d", &particle->orientation.direction);
    fscanf(fp, "%d", &value);
    if (value == 1)
    {
        particle->orientation.mirrored = true;
    }
    else
    {
        particle->orientation.mirrored = false;
    }
    fscanf(fp, "%s", buf);
    particle->vPosition.x = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vPosition.y = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vPosition.z = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vVelocity.x = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vVelocity.y = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vVelocity.z = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vForces.x = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vForces.y = (float)atof(buf);
    fscanf(fp, "%s", buf);
    particle->vForces.z = (float)atof(buf);
    return(particle);
}


// Write particle.
void Particle::write(FILE *fp, Particle *particle)
{
    fprintf(fp, "%d ", particle->id);
    fprintf(fp, "%d ", particle->type);
    fprintf(fp, "%d ", particle->state);
    fprintf(fp, "%f %f %f %f ", particle->fRadius,
        particle->fMass, particle->fCharge,
        particle->coefficientOfRestitution);
    if (particle->orientation.mirrored)
    {
        fprintf(fp, "%d 1 ", particle->orientation.direction);
    }
    else
    {
        fprintf(fp, "%d 0 ", particle->orientation.direction);
    }
    fprintf(fp, "%f %f %f ", particle->vPosition.x,
        particle->vPosition.y, particle->vPosition.z);
    fprintf(fp, "%f %f %f ", particle->vVelocity.x,
        particle->vVelocity.y, particle->vVelocity.z);
    fprintf(fp, "%f %f %f ", particle->vForces.x,
        particle->vForces.y, particle->vForces.z);
    fflush(fp);
}
