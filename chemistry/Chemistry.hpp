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

#ifndef __CHEMISTRY__
#define __CHEMISTRY__

#include "../util/Random.hpp"
#include "../base/Physics.hpp"
#include "Reaction.hpp"
#include "Neighborhood.hpp"

// Chemistry.
class Chemistry
{
    public:

        // Physics.
        Physics *physics;

        // Chemical reactions.
        int numReactions;
        Reaction **reactions;

        // Constructor.
        Chemistry();

        // Destructor.
        ~Chemistry();

        // Initialize.
        void init(Physics *physics);

        // Step chemistry.
        void step();

        // Load and save chemistry.
        void load(FILE *fp);
        void save(FILE *fp);

    private:

        // Particle reactions.
        void react(Neighborhood *neighbors);
};
#endif
