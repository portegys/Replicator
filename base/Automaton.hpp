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
 * Automaton containing artificial chemistry.
 */

#ifndef __AUTOMATON__
#define __AUTOMATON__

#include "Physics.hpp"
#include "../chemistry/Chemistry.hpp"

class Automaton
{
    public:

        // Chemistry.
        Chemistry chemistry;

        // Physics.
        Physics physics;

        // Constructor.
        Automaton();

        // Step system.
        void step();

        // Load and save system.
        void load(FILE *fp);
        void save(FILE *fp);
};
#endif
