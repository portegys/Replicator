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
 * Particle neighborhood.
 * This is a rotatable/mirrorable Moore neighborhood in which the
 * diagonal cells of an obliquely oriented neighborhood is compacted
 * into the same size neighborhood as a perpendicularly oriented one.
 */

#ifndef __NEIGHBORHOOD__
#define __NEIGHBORHOOD__

#include "../base/Particle.hpp"
#include <list>

class Neighborhood
{
    public:

        // The neighborhood origin (0,0) is mapped to the center cell
        // in the matrix.
        std::list<Particle *> particles[3][3];

        // Constructor.
        Neighborhood();

        // Clear.
        void clear();

        // Transform neighborhood by given orientation.
        void transform(Orientation &orientation);

        // Get real cell location given input location and current transform state.
        void getCellLocation(int &x, int &y);

        // Get offsets to cell clockwise steps from given cell in neighborhood.
        // Given cell location must be in neighborhood: -1 <= x,y <= 1
        static void getDxy(int x, int y, Orientation steps, int &dx, int &dy);

    private:

        Orientation orientation;
};
#endif
