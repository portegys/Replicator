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
 * Random numbers.
 */

#include "Random.hpp"

#ifdef UNIX
#include <stdlib.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif

// Set random seed.
void Random::setRand(long seed)
{
    #ifdef UNIX
    srand48(seed);
    #endif
    #ifdef WIN32
    srand(seed);
    #endif
}


// Random boolean.
bool Random::nextBoolean()
{
    int r;
    #ifdef UNIX
    r = lrand48() % 2;
    #endif
    #ifdef WIN32
    r = rand() % 2;
    #endif
    if (r == 1) return true; else return false;
}


// Random float >= 0.0f&& < 1.0f
float Random::nextFloat()
{
    return((float)nextDouble());
}


// Random double >= 0.0 && < 1.0
double Random::nextDouble()
{
    #ifdef UNIX
    return(drand48());
    #endif
    #ifdef WIN32
    return((double)rand() / (double)RAND_MAX);
    #endif
}


// Random integer.
long Random::nextInt()
{
    #ifdef UNIX
    return(lrand48());
    #endif
    #ifdef WIN32
    return(rand());
    #endif
}


// Random integer modulus given value.
long Random::nextInt(int modulus)
{
    #ifdef UNIX
    return(lrand48() % modulus);
    #endif
    #ifdef WIN32
    return(rand() % modulus);
    #endif
}
