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

/**
 * Replicator chemistry.
 *
 * The purpose of this experiment is to investigate the properties of a
 * molecule that replicates under the influence of a catalyst.
 *
 * Usage:
 * Replicator -cycles <reaction cycles>
 *    [-numReplicators <number of replicator molecules>]
 *    [-numCatalysts <number of catalysts>]
 *    [-numComponents <number of free components>]
 *    [-input <input file name> (for run continuation)]
 *    [-output <output file name> (to save run)]
 *    [-logfile <log file name>]
 *    [-display (GUI)]
 *    [-pause (start in pause mode)]
 */

#include "../util/Driver.h"

// Particle types.
#define A_TYPE 0
#define B_TYPE 1
#define C_TYPE 2
#define D_TYPE 3
#define W_TYPE 4
#define X_TYPE 5
#define Y_TYPE 6
#define Z_TYPE 7
#define CATALYST_TYPE 8
#define NUM_PARTICLE_TYPES 9

// Particle colors.
struct ParticleColor
{
    float r,g,b;
} ParticleColors[NUM_PARTICLE_TYPES];

// States.
#define FREE_STATE 0
#define BOND_STATE 1
#define HANDOFF_STATE 2
#define UNBOND_STATE 3

// Usage.
char *Usage = "Replicator -cycles <reaction cycles>\n\t[-numReplicators <number of replicator molecules>]\n\t[-numCatalysts <number of catalysts>]\n\t[-numComponents <number of free components>]\n\t[-input <input file name> (for run continuation)]\n\t[-output <output file name> (to save run)]\n\t[-logfile <log file name>]\n\t[-display (GUI)]\n\t[-pause (start in pause mode)]";

// Quantities.
int NumReplicators;
int NumCatalysts;
int NumComponents;

// Create reactions.
void createReactions();

// Create particles.
void createParticles(int, int, int);
#define MAX_PLACEMENT_TRIES 1000
int placeMap[WIDTH][HEIGHT];

int main(int argc, char *argv[])
{
    int i;

    // Logging to print.
    Log::LOGGING_FLAG = LOG_TO_PRINT;

    // Parse arguments.
    Cycles = -1;
    InputFileName = OutputFileName = NULL;
    NumReplicators = NumCatalysts = NumComponents = 0;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-cycles") == 0)
        {
            i++;
            Cycles = atoi(argv[i]);
            if (Cycles < 0)
            {
                sprintf(Log::messageBuf, "%s: invalid cycles", argv[0]);
                Log::logError();
                exit(1);
            }
            continue;
        }

        if (strcmp(argv[i], "-numReplicators") == 0)
        {
            i++;
            NumReplicators = atoi(argv[i]);
            continue;
        }

        if (strcmp(argv[i], "-numCatalysts") == 0)
        {
            i++;
            NumCatalysts = atoi(argv[i]);
            continue;
        }

        if (strcmp(argv[i], "-numComponents") == 0)
        {
            i++;
            NumComponents = atoi(argv[i]);
            continue;
        }

        if (strcmp(argv[i], "-input") == 0)
        {
            i++;
            InputFileName = argv[i];
            continue;
        }

        if (strcmp(argv[i], "-output") == 0)
        {
            i++;
            OutputFileName = argv[i];
            continue;
        }

        if (strcmp(argv[i], "-logfile") == 0)
        {
            i++;
            Log::LOGGING_FLAG = LOG_TO_BOTH;
            Log::setLogFileName(argv[i]);
            continue;
        }

        if (strcmp(argv[i], "-display") == 0)
        {
            Display = true;
            continue;
        }

        if (strcmp(argv[i], "-pause") == 0)
        {
            Pause = true;
            continue;
        }

        if (strcmp(argv[i], "-help") == 0 ||
            strcmp(argv[i], "--help") == 0 ||
            strcmp(argv[i], "-?") == 0)
        {
            sprintf(Log::messageBuf, "\nUsage: %s", Usage);
            Log::logInformation();
            exit(0);
        }

        sprintf(Log::messageBuf, "\nUsage: %s", Usage);
        Log::logError();
        exit(1);
    }

    if (Cycles < 0)
    {
        sprintf(Log::messageBuf, "\nUsage: %s", Usage);
        Log::logError();
        exit(1);
    }

    if (!Display && Pause)
    {
        sprintf(Log::messageBuf, "\nPause option only valid with display");
        Log::logError();
        sprintf(Log::messageBuf, "\nUsage: %s", Usage);
        Log::logError();
        exit(1);
    }

    if (InputFileName == NULL)
    {
        if (NumReplicators < 0 || NumCatalysts < 0 || NumComponents < 0)
        {
            sprintf(Log::messageBuf, "\nUsage: %s", Usage);
            Log::logError();
            exit(1);
        }
    }
    else
    {
        if (NumReplicators > 0 || NumCatalysts > 0 || NumComponents > 0)
        {
            sprintf(Log::messageBuf, "\nQuantities invalid with continued run");
            Log::logError();
            sprintf(Log::messageBuf, "\nUsage: %s", Usage);
            Log::logError();
            exit(1);
        }
    }

    // Create particle colors.
    for (i = 0; i < NUM_PARTICLE_TYPES; i++)
    {
        ParticleColors[i].r = ((float)Random::nextDouble() * 0.5f) + 0.5f;
        ParticleColors[i].g = ((float)Random::nextDouble() * 0.5f) + 0.5f;
        ParticleColors[i].b = ((float)Random::nextDouble() * 0.5f) + 0.5f;
    }

    // Seed random numbers.
    Random::setRand(time(NULL));

    // Create automaton containing chemistry.
    automaton = new Automaton();
    assert(automaton != NULL);

    // Create reactions.
    createReactions();

    // Initialize run.
    if (InputFileName == NULL)
    {
        // Create particles.
        createParticles(NumReplicators,
            NumCatalysts, NumComponents);
    }
    else
    {
        // Continue run.
        load(InputFileName);
    }

    // Run.
    return driver(argc, argv, "Replicator");
}


// Create reactions.
void createReactions()
{
    int n;
    Reaction *reaction;

    // Allocate reactions.
    automaton->chemistry.numReactions = 62;
    automaton->chemistry.reactions =
        new Reaction*[automaton->chemistry.numReactions];
    assert(automaton->chemistry.reactions != NULL);
    for (n = 0; n < automaton->chemistry.numReactions; n++)
    {
        automaton->chemistry.reactions[n] = new Reaction();
        assert(automaton->chemistry.reactions[n] != NULL);
    }
    n = 0;

    // Particle A:

    // Present catalyst to B.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = A_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = SOUTHWEST;
    reaction->targetBond = NORTHEAST;
    #if ( TRAP == 1 )
    reaction->trap = true;
    reaction->trapNum = n - 1;
    #endif

    // Handoff catalyst to B.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][0] = CATALYST_TYPE;
    reaction->types[1][1] = A_TYPE;
    reaction->types[1][0] = B_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][0] = HANDOFF_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceState = UNBOND_STATE;
    reaction->sourceBond = SOUTHWEST;

    // Unbond from W.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = A_TYPE;
    reaction->types[2][1] = W_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = A_TYPE;
    reaction->types[2][1] = W_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceState = UNBOND_STATE;
    reaction->sourceBond = EAST;

    // Bond to free W.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = A_TYPE;
    reaction->types[2][1] = W_TYPE;
    reaction->types[1][0] = B_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = FREE_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = EAST;
    reaction->targetBond = WEST;

    // Create A-B bond.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = A_TYPE;
    reaction->types[1][0] = B_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 1; reaction->y = 0;
    reaction->sourceBond = SOUTH;
    reaction->targetBond = NORTH;

    // Particle B:

    // Bond to catalyst presented by A.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = B_TYPE;
    reaction->types[1][2] = A_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][2] = HANDOFF_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = WEST;
    reaction->targetBond = EAST;

    // Present catalyst to C.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = B_TYPE;
    reaction->types[1][2] = A_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceBond = SOUTHWEST;
    reaction->targetBond = NORTHEAST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = B_TYPE;
    reaction->types[1][2] = A_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;

    // Handoff catalyst to C.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][0] = CATALYST_TYPE;
    reaction->types[1][1] = B_TYPE;
    reaction->types[1][0] = C_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][0] = HANDOFF_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = SOUTHWEST;
    reaction->sourceState = UNBOND_STATE;

    // Unbond from X.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = B_TYPE;
    reaction->types[2][1] = X_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = B_TYPE;
    reaction->types[2][1] = X_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;
    reaction->sourceState = UNBOND_STATE;

    // Bond to free X.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = B_TYPE;
    reaction->types[2][1] = X_TYPE;
    reaction->types[1][0] = C_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = FREE_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = EAST;
    reaction->targetBond = WEST;

    // Create B-C bond.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = B_TYPE;
    reaction->types[1][0] = C_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 1; reaction->y = 0;
    reaction->sourceBond = SOUTH;
    reaction->targetBond = NORTH;

    // Particle C:

    // Bond to catalyst presented by B.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = C_TYPE;
    reaction->types[1][2] = B_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][2] = HANDOFF_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = WEST;
    reaction->targetBond = EAST;

    // Present catalyst to D.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = C_TYPE;
    reaction->types[1][2] = B_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceBond = SOUTHWEST;
    reaction->targetBond = NORTHEAST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = C_TYPE;
    reaction->types[1][2] = B_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;

    // Handoff catalyst to D.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][0] = CATALYST_TYPE;
    reaction->types[1][1] = C_TYPE;
    reaction->types[1][0] = D_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][0] = HANDOFF_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = SOUTHWEST;
    reaction->sourceState = UNBOND_STATE;

    // Unbond from Y.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = C_TYPE;
    reaction->types[2][1] = Y_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = C_TYPE;
    reaction->types[2][1] = Y_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;
    reaction->sourceState = UNBOND_STATE;

    // Bond to free Y.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = C_TYPE;
    reaction->types[2][1] = Y_TYPE;
    reaction->types[1][0] = D_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = FREE_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = EAST;
    reaction->targetBond = WEST;

    // Create C-D bond.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = C_TYPE;
    reaction->types[1][0] = D_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 1; reaction->y = 0;
    reaction->sourceBond = SOUTH;
    reaction->targetBond = NORTH;

    // Particle D:

    // Bond to catalyst presented by C.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = D_TYPE;
    reaction->types[1][2] = C_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][2] = HANDOFF_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = WEST;
    reaction->targetBond = EAST;

    // Wait for handoff by C
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[0][1] = CATALYST_TYPE;
    reaction->types[1][1] = D_TYPE;
    reaction->types[1][2] = C_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;
    reaction->sourceState = UNBOND_STATE;

    // Unbond from Z.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = D_TYPE;
    reaction->types[2][1] = Z_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = D_TYPE;
    reaction->types[2][1] = Z_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[2][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;
    reaction->sourceState = UNBOND_STATE;

    // Bond to free Z.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = D_TYPE;
    reaction->types[2][1] = Z_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[2][1] = FREE_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = EAST;
    reaction->targetBond = WEST;

    // Particle W:

    // Present catalyst to X.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = W_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = SOUTHEAST;
    reaction->targetBond = NORTHWEST;

    // Handoff catalyst to X.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][0] = CATALYST_TYPE;
    reaction->types[1][1] = W_TYPE;
    reaction->types[1][0] = X_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][0] = HANDOFF_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = SOUTHEAST;
    reaction->sourceState = UNBOND_STATE;

    // Unbond from A.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = W_TYPE;
    reaction->types[0][1] = A_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = W_TYPE;
    reaction->types[0][1] = A_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;
    reaction->sourceState = UNBOND_STATE;

    // Bond to free A.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = W_TYPE;
    reaction->types[0][1] = A_TYPE;
    reaction->types[1][0] = X_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = FREE_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = WEST;
    reaction->targetBond = EAST;

    // Create W-X bond.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = W_TYPE;
    reaction->types[1][0] = X_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 1; reaction->y = 0;
    reaction->sourceBond = SOUTH;
    reaction->targetBond = NORTH;

    // Particle X:

    // Bond to catalyst presented by W.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = X_TYPE;
    reaction->types[1][2] = W_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][2] = HANDOFF_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = EAST;
    reaction->targetBond = WEST;

    // Present catalyst to Y.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = X_TYPE;
    reaction->types[1][2] = W_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceBond = SOUTHEAST;
    reaction->targetBond = NORTHWEST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = X_TYPE;
    reaction->types[1][2] = W_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;

    // Handoff catalyst to Y.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][0] = CATALYST_TYPE;
    reaction->types[1][1] = X_TYPE;
    reaction->types[1][0] = Y_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][0] = HANDOFF_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = SOUTHEAST;
    reaction->sourceState = UNBOND_STATE;

    // Unbond from B.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = X_TYPE;
    reaction->types[0][1] = B_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = X_TYPE;
    reaction->types[0][1] = B_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;
    reaction->sourceState = UNBOND_STATE;

    // Bond to free B.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = X_TYPE;
    reaction->types[0][1] = B_TYPE;
    reaction->types[1][0] = Y_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = FREE_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = WEST;
    reaction->targetBond = EAST;

    // Create X-Y bond.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = X_TYPE;
    reaction->types[1][0] = Y_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 1; reaction->y = 0;
    reaction->sourceBond = SOUTH;
    reaction->targetBond = NORTH;

    // Particle Y:

    // Bond to catalyst presented by X.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[1][2] = X_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][2] = HANDOFF_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = EAST;
    reaction->targetBond = WEST;

    // Present catalyst to Z.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[1][2] = X_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceBond = SOUTHEAST;
    reaction->targetBond = NORTHWEST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[1][2] = X_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;

    // Handoff catalyst to Z.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][0] = CATALYST_TYPE;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[1][0] = Z_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][0] = HANDOFF_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = SOUTHEAST;
    reaction->sourceState = UNBOND_STATE;

    // Unbond from C.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[0][1] = C_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[0][1] = C_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;
    reaction->sourceState = UNBOND_STATE;

    // Bond to free C.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[0][1] = C_TYPE;
    reaction->types[1][0] = Z_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = FREE_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = WEST;
    reaction->targetBond = EAST;

    // Create Y-Z bond.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = Y_TYPE;
    reaction->types[1][0] = Z_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][0] = BOND_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 1; reaction->y = 0;
    reaction->sourceBond = SOUTH;
    reaction->targetBond = NORTH;

    // Particle Z:

    // Bond to catalyst presented by Y.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = Z_TYPE;
    reaction->types[1][2] = Y_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[1][2] = HANDOFF_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 2; reaction->y = 1;
    reaction->sourceState = HANDOFF_STATE;
    reaction->sourceBond = EAST;
    reaction->targetBond = WEST;

    // Wait for handoff by Y
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[2][1] = CATALYST_TYPE;
    reaction->types[1][1] = Z_TYPE;
    reaction->types[1][2] = Y_TYPE;
    reaction->states[1][1] = HANDOFF_STATE;
    reaction->states[1][2] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = EAST;
    reaction->sourceState = UNBOND_STATE;

    // Unbond from D.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = Z_TYPE;
    reaction->types[0][1] = D_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;

    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = Z_TYPE;
    reaction->types[0][1] = D_TYPE;
    reaction->states[1][1] = BOND_STATE;
    reaction->states[0][1] = UNBOND_STATE;
    reaction->reactionType = UNBOND_REACTION;
    reaction->x = 1; reaction->y = 1;
    reaction->sourceBond = WEST;
    reaction->sourceState = UNBOND_STATE;

    // Bond to free D.
    reaction = automaton->chemistry.reactions[n]; n++;
    reaction->types[1][1] = Z_TYPE;
    reaction->types[0][1] = D_TYPE;
    reaction->states[1][1] = UNBOND_STATE;
    reaction->states[0][1] = FREE_STATE;
    reaction->reactionType = BOND_REACTION;
    reaction->x = 0; reaction->y = 1;
    reaction->sourceState = BOND_STATE;
    reaction->targetState = BOND_STATE;
    reaction->sourceBond = WEST;
    reaction->targetBond = EAST;
}


// Configure particles.
void createParticles(int numReplicators,
int numCatalysts, int numComponents)
{
    int i,j,k,dx,dy;
    Particle *a,*b,*c,*d,*w,*x,*y,*z;

    // Clear placement placeMap.
    for (dx = 0; dx < WIDTH; dx++)
    {
        for (dy = 0; dy < HEIGHT; dy++)
        {
            placeMap[dx][dy] = -1;
        }
    }

    // Create replicators.
    for (i = 0; i < numReplicators; i++)
    {
        for (j = 0; j < MAX_PLACEMENT_TRIES; j++)
        {
            dx = Random::nextInt(WIDTH - 1);
            dy = Random::nextInt(HEIGHT - 3) + 3;
            if (placeMap[dx][dy] == -1 && placeMap[dx][dy-1] == -1 &&
                placeMap[dx][dy-2] == -1 && placeMap[dx][dy-3] == -1 &&
                placeMap[dx+1][dy] == -1 && placeMap[dx+1][dy-1] == -1 &&
                placeMap[dx+1][dy-2] == -1 && placeMap[dx+1][dy-3] == -1)
            {
                placeMap[dx][dy] = A_TYPE;
                placeMap[dx][dy-1] = B_TYPE;
                placeMap[dx][dy-2] = C_TYPE;
                placeMap[dx][dy-3] = D_TYPE;
                placeMap[dx+1][dy] = W_TYPE;
                placeMap[dx+1][dy-1] = X_TYPE;
                placeMap[dx+1][dy-2] = Y_TYPE;
                placeMap[dx+1][dy-3] = Z_TYPE;
                break;
            }
        }
        if (j == MAX_PLACEMENT_TRIES) break;

        // Left half.
        a = automaton->physics.createParticle(A_TYPE);
        a->state = BOND_STATE;
        a->vPosition.x = POSITION(dx);
        a->vPosition.y = POSITION(dy);
        b = automaton->physics.createParticle(B_TYPE);
        b->state = BOND_STATE;
        b->vPosition.x = POSITION(dx);
        b->vPosition.y = POSITION(dy - 1);
        automaton->physics.createBond(a, SOUTH, b, NORTH);
        c = automaton->physics.createParticle(C_TYPE);
        c->state = BOND_STATE;
        c->vPosition.x = POSITION(dx);
        c->vPosition.y = POSITION(dy - 2);
        automaton->physics.createBond(b, SOUTH, c, NORTH);
        d = automaton->physics.createParticle(D_TYPE);
        d->state = BOND_STATE;
        d->vPosition.x = POSITION(dx);
        d->vPosition.y = POSITION(dy - 3);
        automaton->physics.createBond(c, SOUTH, d, NORTH);

        // Right half.
        w = automaton->physics.createParticle(W_TYPE);
        w->state = BOND_STATE;
        w->vPosition.x = POSITION(dx + 1);
        w->vPosition.y = POSITION(dy);
        x = automaton->physics.createParticle(X_TYPE);
        x->state = BOND_STATE;
        x->vPosition.x = POSITION(dx + 1);
        x->vPosition.y = POSITION(dy - 1);
        automaton->physics.createBond(w, SOUTH, x, NORTH);
        y = automaton->physics.createParticle(Y_TYPE);
        y->state = BOND_STATE;
        y->vPosition.x = POSITION(dx + 1);
        y->vPosition.y = POSITION(dy - 2);
        automaton->physics.createBond(x, SOUTH, y, NORTH);
        z = automaton->physics.createParticle(Z_TYPE);
        z->state = BOND_STATE;
        z->vPosition.x = POSITION(dx + 1);
        z->vPosition.y = POSITION(dy - 3);
        automaton->physics.createBond(y, SOUTH, z, NORTH);

        automaton->physics.createBond(a, EAST, w, WEST);
        automaton->physics.createBond(b, EAST, x, WEST);
        automaton->physics.createBond(c, EAST, y, WEST);
        automaton->physics.createBond(d, EAST, z, WEST);
    }

    // Create catalysts.
    for (i = 0; i < numCatalysts; i++)
    {
        for (j = 0; j < MAX_PLACEMENT_TRIES; j++)
        {
            dx = Random::nextInt(WIDTH - 1);
            dy = Random::nextInt(HEIGHT - 3) + 3;
            if (placeMap[dx][dy] == -1)
            {
                placeMap[dx][dy] = CATALYST_TYPE;
                break;
            }
        }
        if (j == MAX_PLACEMENT_TRIES) break;

        c = automaton->physics.createParticle(CATALYST_TYPE);
        c->vPosition.x = POSITION(dx);
        c->vPosition.y = POSITION(dy);
    }

    // Create components.
    for (i = 0; i < numComponents; i++)
    {
        for (j = 0; j < MAX_PLACEMENT_TRIES; j++)
        {
            dx = Random::nextInt(WIDTH - 1);
            dy = Random::nextInt(HEIGHT - 3) + 3;
            if (placeMap[dx][dy] == -1)
            {
                k = Random::nextInt(NUM_PARTICLE_TYPES - 1);
                placeMap[dx][dy] = k;
                break;
            }
        }
        if (j == MAX_PLACEMENT_TRIES) break;

        x = automaton->physics.createParticle(k);
        x->state = FREE_STATE;
        x->vPosition.x = POSITION(dx);
        x->vPosition.y = POSITION(dy);
    }
}


// Get particle display specification.
void appGetParticleDisplay(Particle *particle,
struct ParticleDisplay &display)
{
    switch(particle->type)
    {
        case 0: display.label = 'A'; break;
        case 1: display.label = 'B'; break;
        case 2: display.label = 'C'; break;
        case 3: display.label = 'D'; break;
        case 4: display.label = 'W'; break;
        case 5: display.label = 'X'; break;
        case 6: display.label = 'Y'; break;
        case 7: display.label = 'Z'; break;
        case 8: display.label = '*'; break;
        default: display.label = particle->type  + '0'; break;
    }
    display.r = ParticleColors[particle->type].r;
    display.g = ParticleColors[particle->type].g;
    display.b = ParticleColors[particle->type].b;
    display.solid = true;
}


// Get bond display specification.
void appGetBondDisplay(Bond *bond,
struct BondDisplay &display)
{
    display.r = display.g = display.b = 0.0f;
}


// Termination hook.
void appTerminate(int code)
{
    Particle *particle;
    int replicatorCount,strandCount;

    // Count molecules.
    replicatorCount = strandCount = 0;
    for (particle = automaton->physics.particles;
        particle != NULL; particle = particle->next)
    {
        if (particle->type == A_TYPE)
        {
            if (particle->state == BOND_STATE)
            {
                replicatorCount++;
            } else if (particle->state == UNBOND_STATE)
            {
                strandCount++;
            }
        }
    }
    sprintf(Log::messageBuf, "Terminating: replicators=%d strands=%d (pairs)",
        replicatorCount, strandCount);
    Log::logInformation();

    // Save run.
    if (OutputFileName != NULL) save(OutputFileName);
}


#if ( TRAP == 1 )
// Event trap.
void appTrap(int eventNum)
{
    Pause = true;
}
#endif
