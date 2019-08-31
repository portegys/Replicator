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
 * Chemistry.
 */

#include "Chemistry.hpp"

// Constructor.
Chemistry::Chemistry()
{
    numReactions = 0;
    reactions = NULL;
}


// Constructor.
Chemistry::~Chemistry()
{
    for (int i = 0; i < numReactions; i++)
    {
        delete reactions[i];
        reactions[i] = NULL;
    }
    if (reactions != NULL) delete reactions;
    reactions = NULL;
}


// Initialize.
void Chemistry::init(Physics *physics)
{
    this->physics = physics;
    numReactions = 0;
}


// Step chemistry.
void Chemistry::step()
{
    int x,y;
    float px,py;
    Particle *particle,*particle2;
    Neighborhood neighbors;

    // Step particles.
    for (particle = physics->particles; particle != NULL;
        particle = particle->next)
    {
        for (x = 0; x < 3; x++)
        {
            for (y = 0; y < 3; y++)
            {
                neighbors.particles[x][y].clear();
            }
        }

        // Center particle.
        neighbors.particles[1][1].push_front(particle);

        // Attach neighboring particles.
        px = particle->vPosition.x;
        py = particle->vPosition.y;
        for (particle2 = physics->particles; particle2 != NULL;
            particle2 = particle2->next)
        {
            if (particle == particle2) continue;

            if (particle2->vPosition.x < (px - 0.5f) &&
                particle2->vPosition.x >= (px - 1.5f))
            {
                if (particle2->vPosition.y < (py - 0.5f) &&
                    particle2->vPosition.y >= (py - 1.5f))
                {
                    neighbors.particles[0][0].push_front(particle2);
                    continue;
                }
                if (particle2->vPosition.y >= (py - 0.5f) &&
                    particle2->vPosition.y < (py + 0.5f))
                {
                    neighbors.particles[0][1].push_front(particle2);
                    continue;
                }
                if (particle2->vPosition.y >= (py + 0.5f) &&
                    particle2->vPosition.y < (py + 1.5f))
                {
                    neighbors.particles[0][2].push_front(particle2);
                    continue;
                }
            }

            if (particle2->vPosition.x >= (px - 0.5f) &&
                particle2->vPosition.x < (px + 0.5f))
            {
                if (particle2->vPosition.y < (py - 0.5f) &&
                    particle2->vPosition.y >= (py - 1.5f))
                {
                    neighbors.particles[1][0].push_front(particle2);
                    continue;
                }
                if (particle2->vPosition.y >= (py + 0.5f) &&
                    particle2->vPosition.y < (py + 1.5f))
                {
                    neighbors.particles[1][2].push_front(particle2);
                    continue;
                }
            }

            if (particle2->vPosition.x >= (px + 0.5f) &&
                particle2->vPosition.x < (px + 1.5f))
            {
                if (particle2->vPosition.y < (py - 0.5f) &&
                    particle2->vPosition.y >= (py - 1.5f))
                {
                    neighbors.particles[2][0].push_front(particle2);
                    continue;
                }
                if (particle2->vPosition.y >= (py - 0.5f) &&
                    particle2->vPosition.y < (py + 0.5f))
                {
                    neighbors.particles[2][1].push_front(particle2);
                    continue;
                }
                if (particle2->vPosition.y >= (py + 0.5f) &&
                    particle2->vPosition.y < (py + 1.5f))
                {
                    neighbors.particles[2][2].push_front(particle2);
                    continue;
                }
            }
        }

        // Particle reactions.
        react(&neighbors);
    }
}


// Particle reactions.
void Chemistry::react(Neighborhood *neighbors)
{
    int reactionIndex,x,y;
    Reaction *reaction;
    Particle *particle,*particle2;
    std::list<Particle *>::const_iterator listItr,listItr2;

    // Process particles in neighborhood center.
    for (listItr = neighbors->particles[1][1].begin();
        listItr != neighbors->particles[1][1].end(); listItr++)
    {
        particle = *listItr;

        // Transform neighborhood to particle orientation.
        neighbors->transform(particle->orientation);

        // Perform reactions.
        for (reactionIndex = 0; reactionIndex < numReactions; reactionIndex++)
        {
            reaction = reactions[reactionIndex];
            if (reaction->reactionType == NULL_REACTION) continue;
            if (!reaction->matchNeighborhood(neighbors)) continue;

            // Determine reaction target location.
            x = reaction->x - 1;
            y = reaction->y - 1;
            neighbors->getCellLocation(x, y);
            x++; y++;

            // Create particle?
            if (reaction->reactionType == CREATE_REACTION)
            {
                float px = particle->vPosition.x + float(x - 1);
                if (px < 0.0f || px >= (float)WIDTH) continue;
                float py = particle->vPosition.y + float(y - 1);
                if (py < 0.0f || py >= (float)HEIGHT) continue;
                particle2 = physics->createParticle(reaction->type);
                if (particle2 != NULL)
                {
                    #if ( TRAP == 1 )
                    // Trap event?
                    if (reaction->trap)
                    {
                        appTrap(reaction->trapNum);
                    }
                    #endif
                    particle2->vPosition.x = px;
                    particle2->vPosition.y = py;
                    particle2->vVelocity = particle->vVelocity;

                    // Orient particle.
                    particle2->orientation.direction =
                        particle->orientation.aim(reaction->orientation.direction);
                    particle2->orientation.mirrored =
                        particle->orientation.getMirrorX2(reaction->orientation.mirrored);

                    // Set next states.
                    if (reaction->sourceState != Reaction::IGNORE_STATE)
                    {
                        particle->state = reaction->sourceState;
                    }
                    if (reaction->targetState != Reaction::IGNORE_STATE)
                    {
                        particle2->state = reaction->targetState;
                    }
                }
                continue;
            }

            // Apply remaining reactions to targeted particles.
            for (listItr2 = neighbors->particles[x][y].begin();
                listItr2 != neighbors->particles[x][y].end(); listItr2++)
            {
                particle2 = *listItr2;
                if (particle2->type != reaction->types[x][y]) continue;

                #if ( TRAP == 1 )
                // Trap event?
                if (reaction->trap)
                {
                    appTrap(reaction->trapNum);
                }
                #endif
                // Set next states.
                if (reaction->sourceState != Reaction::IGNORE_STATE)
                {
                    particle->state = reaction->sourceState;
                }
                if (reaction->targetState != Reaction::IGNORE_STATE)
                {
                    particle2->state = reaction->targetState;
                }

                switch(reaction->reactionType)
                {
                    case BOND_REACTION:
                        physics->createBond(particle,
                            particle->orientation.aim(reaction->sourceBond),
                            particle2,
                            particle->orientation.aim(reaction->targetBond),
                            reaction->bondStrength);
                        break;

                    case SET_TYPE_REACTION:
                        particle2->type = reaction->type;
                        break;

                    case SET_STATE_REACTION:
                        break;

                    case ORIENT_REACTION:
                        particle2->orientation.direction =
                            particle->orientation.aim(reaction->orientation.direction);
                        particle2->orientation.mirrored =
                            particle->orientation.getMirrorX2(reaction->orientation.mirrored);
                        break;

                    case UNBOND_REACTION:
                        physics->removeBond(particle2, particle2->orientation.aim(reaction->sourceBond));
                        break;

                    case DESTROY_REACTION:
                        physics->removeParticle(particle2);
                        break;
                }
            }
        }
    }
}


// Load chemistry.
void Chemistry::load(FILE *fp) {}

// Save chemistry.
void Chemistry::save(FILE *fp) {}
