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
 * Chemical reaction.
 */

#ifndef __REACTION__
#define __REACTION__

#include <stdio.h>
#include "Neighborhood.hpp"

// Reaction types:
// Create/destroy, set type/state, bond/unbond, orient body.
#define NULL_REACTION (-1)
#define CREATE_REACTION 0
#define BOND_REACTION   1
#define SET_TYPE_REACTION 2
#define SET_STATE_REACTION 3
#define ORIENT_REACTION 4
#define UNBOND_REACTION 5
#define DESTROY_REACTION 6
#define NUM_REACTION_TYPES 7

// Reaction.
// Pattern matching is done against a Moore (3x3) neighborhood
// of cells centered on a particle. Various reaction types determine
// changes to particles in the neighborhood.
class Reaction
{
    public:

        // Description.
        char *description;
        void setDescription(char *);

        // Matching neighborhood particle types.
        static const int IGNORE_CELL;
        static const int EMPTY_CELL;
        static const int OCCUPIED_CELL;
        int types[3][3];

        // Matching neighborhood particle states.
        static const int IGNORE_STATE;
        int states[3][3];

        // Reaction type.
        int reactionType;

        // Reaction target location in neighborhood.
        int x,y;

        // Next states of reacting particles.
        int sourceState;
        int targetState;

        // Reaction parameters.
        int type;
        Orientation orientation;
        int sourceBond,targetBond;
        float bondStrength;

        // Constructor.
        Reaction();

        // Destructor.
        ~Reaction();

        // Clear reaction.
        void clear();

        // Determine neighborhood match.
        bool matchNeighborhood(Neighborhood *neighborhood);

        // Duplicate reaction.
        Reaction *duplicate();

        // Read reaction.
        static Reaction *read(FILE *fp);

        // Write reaction.
        static void write(FILE *fp, Reaction *reaction);

        // Print reaction.
        void print();

    #if ( TRAP == 1 )
        bool trap;
        int trapNum;
    #endif
};
#endif
