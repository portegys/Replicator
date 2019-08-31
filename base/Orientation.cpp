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

#include "Orientation.hpp"
#include "../util/Math_etc.h"

// Constructors.
Orientation::Orientation()
{
    direction = NORTH;
    mirrored = false;
}


Orientation::Orientation(int direction)
{
    this->direction = direction;
    mirrored = false;
}


Orientation::Orientation(int direction, bool mirrored)
{
    this->direction = direction;
    this->mirrored = mirrored;
}


// Copy.
void Orientation::copy(Orientation *orientation)
{
    orientation->direction = direction;
    orientation->mirrored = mirrored;
}


// Mirror.
void Orientation::mirror(bool mirrored)
{
    this->mirrored = mirrored;
}


// What is mirror of my mirror?
bool Orientation::getMirrorX2(bool mirrored)
{
    if (this->mirrored)
    {
        if (mirrored) return false; else return true;
    }
    else
    {
        if (mirrored) return true; else return false;
    }
}


// Rotate.
void Orientation::rotate(int amount)
{
    direction = offset(amount);
}


// Get aim as an absolute direction.
int Orientation::aim(int amount)
{
    return offset(amount);
}


// Transform coordinates relative to orientation.
void Orientation::transform(int &x, int &y)
{
    double angle;
    double x2,y2;

    switch(direction)
    {
        case NORTH:
            angle = 0.0;
            break;
        case NORTHEAST:
            angle = -45.0;
            break;
        case EAST:
            angle = -90.0;
            break;
        case SOUTHEAST:
            angle = -135.0;
            break;
        case SOUTH:
            angle = -180.0;
            break;
        case SOUTHWEST:
            angle = 135.0;
            break;
        case WEST:
            angle = 90.0;
            break;
        case NORTHWEST:
            angle = 45.0;
            break;
    }
    if (mirrored) angle = -angle;
    angle = DegreesToRadians((float)angle);
    x2 = ((double)x * cos(angle)) - ((double)y * sin(angle));
    y2 = ((double)y * cos(angle)) + ((double)x * sin(angle));
    if (x2 < 0.0) x2 -= 0.00001; else x2 += 0.00001;
    if (y2 < 0.0) y2 -= 0.00001; else y2 += 0.00001;
    x = (int)x2;
    y = (int)y2;
}


int Orientation::offset(int amount)
{
    int i;

    if (mirrored)
    {
        i = direction - amount;
    }
    else
    {
        i = direction + amount;
    }
    while (i < 0) i += 8;
    while (i > 7) i -= 8;
    return(i);
}
