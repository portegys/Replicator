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
 * Bond properties.
 */

#include "Bond.hpp"
#include "Physics.hpp"

// Constructors.
Bond::Bond()
{
    strength = DEFAULT_BOND_STRENGTH;
}


Bond::Bond(float strength)
{
    this->strength = strength;
}


// Destructor.
Bond::~Bond() {}

// Get bond strength.
float Bond::getStrength()
{
    return strength;
}
