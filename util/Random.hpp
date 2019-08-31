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

#ifndef __RANDOM__
#define __RANDOM__

class Random
{
    public:

        // Set random seed.
        static void setRand(long seed);

        // Random boolean.
        static bool nextBoolean();

        // Random float >= 0.0f && < 1.0f
        static float nextFloat();

        // Random double >= 0.0 && < 1.0
        static double nextDouble();

        // Random integer.
        static long nextInt();

        // Random integer modulus given value.
        static long nextInt(int modulus);
};
#endif
