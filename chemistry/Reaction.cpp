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

#include <math.h>
#include <assert.h>
#include <string.h>
#include "Reaction.hpp"
#include "../base/Physics.hpp"

// Special types.
const int Reaction::IGNORE_CELL = -1;
const int Reaction::EMPTY_CELL = -2;
const int Reaction::OCCUPIED_CELL = -3;

// Special states.
const int Reaction::IGNORE_STATE = -1;

// Constructor.
Reaction::Reaction()
{
    clear();
}


// Destructor.
Reaction::~Reaction()
{
    if (description != NULL)
    {
        delete description;
        description = NULL;
    }
}


// Clear reaction values.
void Reaction::clear()
{
    int x,y;

    description = NULL;

    // Clear particle types and states.
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            types[x][y] = IGNORE_CELL;
            states[x][y] = IGNORE_STATE;
        }
    }

    // Initialize default reaction type.
    reactionType = NULL_REACTION;

    // Initialize reacting particle states.
    sourceState = targetState = IGNORE_STATE;

    // Initialize reaction target location.
    x = 0;
    y = 0;

    // Initialize parameters.
    type = 0;
    sourceBond = targetBond = 0;
    bondStrength = DEFAULT_BOND_STRENGTH;

    #if ( TRAP == 1 )
    trap = false;
    trapNum = -1;
    #endif
}


// Set description.
void Reaction::setDescription(char *s)
{
    if (description != NULL) delete description;
    description = new char[strlen(s)+1];
    assert(description != NULL);
    strcpy(description, s);
}


// Determine neighborhood match.
bool Reaction::matchNeighborhood(Neighborhood *neighbors)
{
    int x,y,x2,y2;
    Particle *particle;
    bool match;
    std::list<Particle *>::const_iterator listItr;

    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            if (types[x][y] == IGNORE_CELL) continue;
            x2 = x - 1;
            y2 = y - 1;
            neighbors->getCellLocation(x2, y2);
            x2++; y2++;
            if (types[x][y] == EMPTY_CELL)
            {
                if (neighbors->particles[x2][y2].size() != 0)
                {
                    return false;
                }
            } else if (types[x][y] == OCCUPIED_CELL)
            {
                if (neighbors->particles[x2][y2].size() == 0)
                {
                    return false;
                }
            }
            else
            {
                match = false;
                for (listItr = neighbors->particles[x2][y2].begin();
                    listItr != neighbors->particles[x2][y2].end(); listItr++)
                {
                    particle = *listItr;
                    if (particle->type == types[x][y])
                    {
                        if (states[x][y] == IGNORE_STATE ||
                            states[x][y] == particle->state)
                        {
                            match = true;
                            break;
                        }
                    }
                }
                if (!match) return false;
            }
        }
    }
    return true;
}


// Duplicate reaction.
Reaction *Reaction::duplicate()
{
    int x,y;

    Reaction *reaction = new Reaction();
    assert(reaction != NULL);
    if (description != NULL)
    {
        reaction->description = new char[strlen(description)+1];
        assert(reaction->description != NULL);
        strcpy(reaction->description, description);
    }
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            reaction->types[x][y] = types[x][y];
            reaction->states[x][y] = states[x][y];
        }
    }
    reaction->reactionType = reactionType;
    reaction->x = x;
    reaction->y = y;
    reaction->sourceState = sourceState;
    reaction->targetState = targetState;
    reaction->type = type;
    reaction->orientation = orientation;
    reaction->sourceBond = sourceBond;
    reaction->targetBond = targetBond;
    reaction->bondStrength = bondStrength;
    return(reaction);
}


// Read reaction.
Reaction *Reaction::read(FILE *fp)
{
    int len,x,y,value;
    char buf[50];

    Reaction *reaction = new Reaction();
    assert(reaction != NULL);
    fscanf(fp, "%d", &len);
    if (len > 0)
    {
        fgetc(fp);
        reaction->description = new char[len] + 2;
        assert(reaction->description != NULL);
        fgets(reaction->description , len + 2 , fp);
        reaction->description[len] = '\0';
    }
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            fscanf(fp, "%d", &reaction->types[x][y]);
        }
    }
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            fscanf(fp, "%d", &reaction->states[x][y]);
        }
    }
    fscanf(fp, "%d", &reaction->reactionType);
    fscanf(fp, "%d", &reaction->x);
    fscanf(fp, "%d", &reaction->y);
    fscanf(fp, "%d", &reaction->sourceState);
    fscanf(fp, "%d", &reaction->targetState);
    fscanf(fp, "%d", &reaction->type);
    fscanf(fp, "%d", &reaction->orientation.direction);
    fscanf(fp, "%d", &value);
    if (value == 1)
    {
        reaction->orientation.mirrored = true;
    }
    else
    {
        reaction->orientation.mirrored = false;
    }
    fscanf(fp, "%d", &reaction->sourceBond);
    fscanf(fp, "%d", &reaction->targetBond);
    fscanf(fp, "%s", buf);
    reaction->bondStrength = (float)atof(buf);
    fgetc(fp);
    return(reaction);
}


// Write reaction.
void Reaction::write(FILE *fp, Reaction *reaction)
{
    int x,y;

    if (reaction->description == NULL)
    {
        fprintf(fp, "0\n");
    }
    else
    {
        fprintf(fp, "%d %s\n", strlen(reaction->description),
            reaction->description);
    }
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            fprintf(fp, "%d ", reaction->types[x][y]);
        }
    }
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            fprintf(fp, "%d ", reaction->states[x][y]);
        }
    }
    fprintf(fp, "%d ", reaction->reactionType);
    fprintf(fp, "%d ", reaction->x);
    fprintf(fp, "%d ", reaction->y);
    fprintf(fp, "%d ", reaction->sourceState);
    fprintf(fp, "%d ", reaction->targetState);
    fprintf(fp, "%d ", reaction->type);
    fprintf(fp, "%d ", reaction->orientation.direction);
    if (reaction->orientation.mirrored)
    {
        fprintf(fp, "1 ");
    }
    else
    {
        fprintf(fp, "0 ");
    }
    fprintf(fp, "%d ", reaction->sourceBond);
    fprintf(fp, "%d ", reaction->targetBond);
    fprintf(fp, "%s\n", reaction->bondStrength);
    fflush(fp);
}


// Print reaction.
void Reaction::print()
{
    int x,y;

    printf("description: ");
    if (description == NULL)
    {
        printf("<empty> ");
    }
    else
    {
        printf("%s ", description);
    }
    printf("types: {");
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            printf("%d ", types[x][y]);
        }
    }
    printf("}");
    printf(" states: {");
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            printf("%d ", states[x][y]);
        }
    }
    printf("}");
    printf(" reaction type=%d", reactionType);
    printf(" location=%d,%d", x, y);
    printf(" source state=%d", sourceState);
    printf(" target state=%d", targetState);
    printf(" parameters: {");
    printf("type=%d ", type);
    printf("orientation {%d/", orientation.direction);
    if (orientation.mirrored)
    {
        printf("true");
    }
    else
    {
        printf("false");
    }
    printf("}");
    printf(" source bond=%d", sourceBond);
    printf(" target bond=%d", targetBond);
    printf(" bond strength=%f", bondStrength);
    printf("}");
    printf("\n");
}
