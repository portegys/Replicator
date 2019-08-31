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
 */

#include <assert.h>
#include "Neighborhood.hpp"

// Neighborhood constructor.
Neighborhood::Neighborhood()
{
    clear();
}


// Clear neighborhood.
void Neighborhood::clear()
{
    int x,y;

    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            particles[x][y].clear();
        }
    }
}


// Transform neighborhood.
void Neighborhood::transform(Orientation &orientation)
{
    this->orientation = orientation;
}


// Get real cell location given input location and current transform state.
void Neighborhood::getCellLocation(int &x, int &y)
{
    int x2,y2,xret,yret,dir,r;

    dir = orientation.direction;
    if (orientation.mirrored)
    {
        switch(dir)
        {
            case NORTH: break;
            case NORTHEAST: dir = NORTHWEST; break;
            case EAST: dir = WEST; break;
            case SOUTHEAST: dir = SOUTHWEST; break;
            case SOUTH: break;
            case SOUTHWEST: dir = SOUTHEAST; break;
            case WEST: dir = EAST; break;
            case NORTHWEST: dir = NORTHEAST; break;
        }
    }

    xret = yret = 0;
    switch(dir)
    {
        case NORTH:
        {
            xret = x;
            yret = y;
        }
        break;

        case NORTHEAST:
        {
            if (x > 0)
            {
                for (x2 = 0; x2 < x; x2++)
                {
                    xret++; yret--;
                }
            }
            else
            {
                for (x2 = 0; x2 > x; x2--)
                {
                    xret--; yret++;
                }
            }
            if (y > 0)
            {
                for (y2 = 0; y2 < y; y2++)
                {
                    xret++; yret++;
                }
            }
            else
            {
                for (y2 = 0; y2 > y; y2--)
                {
                    xret--; yret--;
                }
            }
        }
        break;

        case EAST:
        {
            if (x > 0)
            {
                for (x2 = 0; x2 < x; x2++)
                {
                    yret--;
                }
            }
            else
            {
                for (x2 = 0; x2 > x; x2--)
                {
                    yret++;
                }
            }
            if (y > 0)
            {
                for (y2 = 0; y2 < y; y2++)
                {
                    xret++;
                }
            }
            else
            {
                for (y2 = 0; y2 > y; y2--)
                {
                    xret--;
                }
            }
        }
        break;

        case SOUTHEAST:
        {
            if (x > 0)
            {
                for (x2 = 0; x2 < x; x2++)
                {
                    xret--; yret--;
                }
            }
            else
            {
                for (x2 = 0; x2 > x; x2--)
                {
                    xret++; yret++;
                }
            }
            if (y > 0)
            {
                for (y2 = 0; y2 < y; y2++)
                {
                    xret++; yret--;
                }
            }
            else
            {
                for (y2 = 0; y2 > y; y2--)
                {
                    xret--; yret++;
                }
            }
        }
        break;

        case SOUTH:
        {
            xret = -x;
            yret = -y;
        }
        break;

        case SOUTHWEST:
        {
            if (x > 0)
            {
                for (x2 = 0; x2 < x; x2++)
                {
                    xret--; yret++;
                }
            }
            else
            {
                for (x2 = 0; x2 > x; x2--)
                {
                    xret++; yret--;
                }
            }
            if (y > 0)
            {
                for (y2 = 0; y2 < y; y2++)
                {
                    xret--; yret--;
                }
            }
            else
            {
                for (y2 = 0; y2 > y; y2--)
                {
                    xret++; yret++;
                }
            }
        }
        break;

        case WEST:
        {
            if (x > 0)
            {
                for (x2 = 0; x2 < x; x2++)
                {
                    yret++;
                }
            }
            else
            {
                for (x2 = 0; x2 > x; x2--)
                {
                    yret--;
                }
            }
            if (y > 0)
            {
                for (y2 = 0; y2 < y; y2++)
                {
                    xret--;
                }
            }
            else
            {
                for (y2 = 0; y2 > y; y2--)
                {
                    xret++;
                }
            }
        }
        break;

        case NORTHWEST:
        {
            if (x > 0)
            {
                for (x2 = 0; x2 < x; x2++)
                {
                    xret++; yret++;
                }
            }
            else
            {
                for (x2 = 0; x2 > x; x2--)
                {
                    xret--; yret--;
                }
            }
            if (y > 0)
            {
                for (y2 = 0; y2 < y; y2++)
                {
                    xret--; yret++;
                }
            }
            else
            {
                for (y2 = 0; y2 > y; y2--)
                {
                    xret++; yret--;
                }
            }
        }
        break;
    }

    // Compact.
    if (abs(x) > abs(y)) r = abs(x); else r = abs(y);
    if (abs(xret) > r)
    {
        if (xret > 0)
        {
            xret = r;
        }
        else
        {
            xret = -r;
        }
    }
    if (abs(yret) > r)
    {
        if (yret > 0)
        {
            yret = r;
        }
        else
        {
            yret = -r;
        }
    }
    x = xret; y = yret;
}


// Get offsets to cell clockwise steps from given cell in neighborhood.
void Neighborhood::getDxy(int x, int y, Orientation steps, int &dx, int &dy)
{
    int dir,dir2;

    assert(x >= -1 && x <= 1 && y >= -1 && y <= 1);
    switch(x)
    {
        case -1:
        {
            switch(y)
            {
                case -1: dir = SOUTHWEST; break;
                case 0: dir = WEST; break;
                case 1: dir = NORTHWEST; break;
            }
        }
        break;
        case 0:
        {
            switch(y)
            {
                case -1: dir = SOUTH; break;
                case 0: dir = CENTER; break;
                case 1: dir = NORTH; break;
            }
        }
        break;
        case 1:
        {
            switch(y)
            {
                case -1: dir = SOUTHEAST; break;
                case 0: dir = EAST; break;
                case 1: dir = NORTHEAST; break;
            }
        }
        break;
    }

    if (dir == CENTER)
    {
        dx = 0;
        dy = 0;
        return;
    }
    else
    {
        if (!steps.mirrored)
        {
            dir2 = dir + steps.direction;
            while (dir2 > 7) dir2 -= 8;
        }
        else
        {
            dir2 = dir - steps.direction;
            while (dir2 < 0) dir2 += 8;
        }
    }

    switch(dir)
    {
        case NORTH:

            switch(dir2)
            {
                case NORTH: dx = dy = 0; break;
                case NORTHEAST: dx = 1; dy = 0; break;
                case EAST: dx = 1; dy = -1; break;
                case SOUTHEAST: dx = 1; dy = -2; break;
                case SOUTH: dx = 0; dy = -2; break;
                case SOUTHWEST: dx = -1; dy = -2; break;
                case WEST: dx = -1; dy = -1; break;
                case NORTHWEST: dx = -1; dy = 0; break;
            }
            break;
        case NORTHEAST:
            switch(dir2)
            {
                case NORTH: dx = -1; dy = 0; break;
                case NORTHEAST: dx = 0; dy = 0; break;
                case EAST: dx = 0; dy = -1; break;
                case SOUTHEAST: dx = 0; dy = -2; break;
                case SOUTH: dx = -1; dy = -2; break;
                case SOUTHWEST: dx = -2; dy = -2; break;
                case WEST: dx = -2; dy = -1; break;
                case NORTHWEST: dx = -2; dy = 0; break;
            }
            break;
        case EAST:
            switch(dir2)
            {
                case NORTH: dx = -1; dy = 1; break;
                case NORTHEAST: dx = 0; dy = 1; break;
                case EAST: dx = 0; dy = 0; break;
                case SOUTHEAST: dx = 0; dy = -1; break;
                case SOUTH: dx = -1; dy = -1; break;
                case SOUTHWEST: dx = -2; dy = -1; break;
                case WEST: dx = -2; dy = 0; break;
                case NORTHWEST: dx = -2; dy = 1; break;
            }
            break;
        case SOUTHEAST:
            switch(dir2)
            {
                case NORTH: dx = -1; dy = 2; break;
                case NORTHEAST: dx = 0; dy = 2; break;
                case EAST: dx = 0; dy = 1; break;
                case SOUTHEAST: dx = 0; dy = 0; break;
                case SOUTH: dx = -1; dy = 0; break;
                case SOUTHWEST: dx = -2; dy = 0; break;
                case WEST: dx = -2; dy = 1; break;
                case NORTHWEST: dx = -2; dy = 2; break;
            }
            break;
        case SOUTH:
            switch(dir2)
            {
                case NORTH: dx = 0; dy = 2; break;
                case NORTHEAST: dx = 1; dy = 2; break;
                case EAST: dx = 1; dy = 1; break;
                case SOUTHEAST: dx = 1; dy = 0; break;
                case SOUTH: dx = 0; dy = 0; break;
                case SOUTHWEST: dx = -1; dy = 0; break;
                case WEST: dx = -1; dy = 1; break;
                case NORTHWEST: dx = -1; dy = 2; break;
            }
            break;
        case SOUTHWEST:
            switch(dir2)
            {
                case NORTH: dx = 1; dy = 2; break;
                case NORTHEAST: dx = 2; dy = 2; break;
                case EAST: dx = 2; dy = 1; break;
                case SOUTHEAST: dx = 2; dy = 0; break;
                case SOUTH: dx = 1; dy = 0; break;
                case SOUTHWEST: dx = 0; dy = 0; break;
                case WEST: dx = 0; dy = 1; break;
                case NORTHWEST: dx = 0; dy = 2; break;
            }
            break;
        case WEST:
            switch(dir2)
            {
                case NORTH: dx = 1; dy = 1; break;
                case NORTHEAST: dx = 2; dy = 1; break;
                case EAST: dx = 2; dy = 0; break;
                case SOUTHEAST: dx = 2; dy = -1; break;
                case SOUTH: dx = 1; dy = -1; break;
                case SOUTHWEST: dx = 0; dy = -1; break;
                case WEST: dx = 0; dy = 0; break;
                case NORTHWEST: dx = 0; dy = 1; break;
            }
            break;
        case NORTHWEST:
            switch(dir2)
            {
                case NORTH: dx = 1; dy = 0; break;
                case NORTHEAST: dx = 2; dy = 0; break;
                case EAST: dx = 2; dy = -1; break;
                case SOUTHEAST: dx = 2; dy = -2; break;
                case SOUTH: dx = 1; dy = -2; break;
                case SOUTHWEST: dx = 0; dy = -2; break;
                case WEST: dx = 0; dy = -1; break;
                case NORTHWEST: dx = 0; dy = 0; break;
            }
            break;
    }
}
