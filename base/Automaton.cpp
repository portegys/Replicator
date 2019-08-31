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
 * Automaton for chemical reactions.
 */

#include "Automaton.hpp"

// Constructor.
Automaton::Automaton()
{
    chemistry.init(&physics);
}


// Step system.
void Automaton::step()
{
    // Perform physics.
    physics.step(DTIME);

    // Perform chemistry.
    chemistry.step();
}


// Load system.
void Automaton::load(FILE *fp)
{
    physics.load(fp);
    chemistry.load(fp);
}


// Save system.
void Automaton::save(FILE *fp)
{
    physics.save(fp);
    chemistry.save(fp);
}
