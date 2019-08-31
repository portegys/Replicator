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
 * Orientation.
 */

#ifndef __ORIENTATION__
#define __ORIENTATION__

#include "Parameters.h"

class Orientation
{
    public:

        int direction;
        bool mirrored;

        // Constructors.
        Orientation();
        Orientation(int direction);
        Orientation(int direction, bool mirrored);

        // Copy.
        void copy(Orientation *orientation);

        // Mirror.
        void mirror(bool mirrored);

        // What is mirror of my mirror?
        bool getMirrorX2(bool mirrored);

        // Rotate.
        void rotate(int amount);

        // Get aim as an absolute direction.
        int aim(int amount);

        // Transform coordinates relative to orientation.
        void transform(int &x, int &y);

    private:

        int offset(int amount);
};
#endif
