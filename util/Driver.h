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

/**
 * Driver utility functions.
 */

#ifdef WIN32
#include <windows.h>
#endif
#ifdef UNIX
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#endif
#include <time.h>
#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "../base/Parameters.h"
#include "../base/Automaton.hpp"
#include "../util/Log.hpp"

#ifdef WIN32
#ifdef _DEBUG
// For Windows memory checking, set CHECK_MEMORY = 1
#define CHECK_MEMORY 0
#if ( CHECK_MEMORY == 1 )
#include <crtdbg.h>
#endif
#endif
#endif

// Automaton.
Automaton *automaton;

// Reaction cycles.
int Cycles;
int CycleCount;

// Files.
char *InputFileName = NULL;
char *OutputFileName = NULL;

// Start/end functions.
void load(char *fileName);
void save(char *fileName);
void terminate(int);

// Display mode?
bool Display = false;

// Draw grid?
bool DrawGrid = false;

// Run flags.
bool Quit = false;
bool Pause = false;
bool Step = false;
bool Progress = true;

// Window size.
#define WINDOW_WIDTH 550
#define WINDOW_HEIGHT 550
int WindowWidth = WINDOW_WIDTH;
int WindowHeight = WINDOW_HEIGHT;
float CellWidth = (float)WindowWidth / (float)WIDTH;
float CellHeight = (float)WindowHeight / (float)HEIGHT;

/*
    Available fonts:
    GLUT_BITMAP_8_BY_13
    GLUT_BITMAP_9_BY_15
    GLUT_BITMAP_TIMES_ROMAN_10
    GLUT_BITMAP_TIMES_ROMAN_24
    GLUT_BITMAP_HELVETICA_10
    GLUT_BITMAP_HELVETICA_12
    GLUT_BITMAP_HELVETICA_18
*/
#define SMALL_FONT GLUT_BITMAP_8_BY_13
#define FONT GLUT_BITMAP_9_BY_15
#define BIG_FONT GLUT_BITMAP_TIMES_ROMAN_24
#define LINE_SPACE 15

// User modes.
typedef enum { RUN, HELP }
USERMODE;
USERMODE UserMode = RUN;

// Control information.
char *ControlInfo[] =
{
    "           h : Control help",
    "           c : Toggle cycle display",
    "           p : Toggle pause mode",
    "           s : Toggle step mode",
    "     <space> : Step",
    "           q : Quit",
    NULL
};

// GUI functions.
void display(), drawParticle(Particle *);
void idle();
void reshape(int, int);
void keyInput(unsigned char key, int x, int y);
void helpInfo();
void renderBitmapString(GLfloat, GLfloat, void *, char *);

// Application supplied functions:

// Particle display.
struct ParticleDisplay
{
    float r,g,b;                                  // For glColor3f.
    bool solid;                                   // Filled circle?
    char label;                                   // Particle label.
};
void appGetParticleDisplay(Particle *, struct ParticleDisplay &);

// Bond display.
struct BondDisplay
{
    float r,g,b;                                  // For glColor3f.

    // For stipple line (see glLineStipple).
    int repeat;                                   // Repeat each bit in pattern.
    unsigned short pattern;                       // Line bit pattern.
};
void appGetBondDisplay(Bond *, struct BondDisplay &);

// Termination.
void appTerminate(int);

#if ( TRAP == 1 )
// Event trapping.
void appTrap(int);
#endif

// Driver.
int driver(int argc, char *argv[], char *appName)
{
    Log::logInformation("Begin reactions:");
    CycleCount = 0;
    if (!Display)
    {
        // Reaction loop.
        for (; CycleCount < Cycles; CycleCount++)
        {
            automaton->step();
        }
        save(OutputFileName);

        terminate(0);

    }                                             // Display.
    else
    {

        // Initialize display.
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
        glutInitWindowSize(WindowWidth, WindowHeight);
        glutCreateWindow(appName);
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keyInput);
        glutIdleFunc(idle);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0f, (float)WindowWidth, 0.0f, (float)WindowHeight);
        glScalef(1, -1, 1);
        glTranslatef(0, -WindowHeight, 0);

        // Start up.
        glutMainLoop();
    }
    return 0;
}


// Load run.
void load(char *fileName)
{
    FILE *fp;

    if (fileName == NULL) return;
    if ((fp = fopen(fileName, "r")) == NULL)
    {
        sprintf(Log::messageBuf, "Cannot load file %s", fileName);
        Log::logError();
        exit(1);
    }
    automaton->load(fp);
    fclose(fp);
}


// Save run.
void save(char *fileName)
{
    FILE *fp;

    if (fileName == NULL) return;
    if ((fp = fopen(fileName, "w")) == NULL)
    {
        sprintf(Log::messageBuf, "Cannot save to file %s", fileName);
        Log::logError();
        exit(1);
    }
    automaton->save(fp);
    fclose(fp);
}


// Terminate.
void terminate(int code)
{
    // Application termination.
    appTerminate(code);

    // Release memory.
    if (automaton != NULL)
    {
        delete automaton;
        automaton = NULL;
    }

    #ifdef WIN32
    #if ( CHECK_MEMORY == 1 )
    // Check for memory leaks.
    HANDLE hFile = CreateFile(                    // Dump to temp log.
        TEMP_LOG_FILE_NAME,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        0,
        NULL
        );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        sprintf(Log::messageBuf, "Cannot open memory check temporary file %s",
            TEMP_LOG_FILE_NAME);
        Log::logError();
        exit(1);
    }
    Log::logInformation("\nMemory leak check output:");
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, hFile);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, hFile );
    if (!_CrtDumpMemoryLeaks())
    {
        Log::logInformation("No memory leaks");
        CloseHandle(hFile);
    }
    else
    {
        CloseHandle(hFile);
        Log::appendTempLog();
    }
    Log::removeTempLog();
    #endif
    #endif

    Log::logInformation("End reactions");
    Log::close();
    exit(code);
}


// Display.
void display()
{
    int i, x, y;
    float x2, y2, x3, y3;
    Particle *particle,*particle2;
    char buf[50];
    struct BondDisplay bondDisplay;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw automaton.
    if (automaton != NULL)
    {
        glBegin(GL_LINES);
        if (DrawGrid)
        {
            y2 = (float)WindowHeight;
            for (x = 1, x2 = CellWidth - 1.0f; x < WIDTH;
                x++, x2 = (CellWidth * (float)x) - 1.0f)
            {
                glVertex2f(x2, 0.0f);
                glVertex2f(x2, y2);
            }
            x2 = (float)WindowWidth;
            for (y = 1, y2 = CellHeight - 1.0f; y < HEIGHT;
                y++, y2 = (CellHeight * (float)y) - 1.0f)
            {
                glVertex2f(0.0f, y2);
                glVertex2f(x2, y2);
            }
        }
        else
        {
            glVertex2f(0.0f, 0.0f);
            glVertex2f((float)WindowWidth, 0.0f);
            glVertex2f(0.0f, (float)WindowHeight - 1.0f);
            glVertex2f((float)WindowWidth, (float)WindowHeight - 1.0f);
        }
        glEnd();

        for (particle = automaton->physics.particles; particle != NULL;
            particle = particle->next)
        {
            drawParticle(particle);
        }
        for (particle = automaton->physics.particles; particle != NULL;
            particle = particle->next)
        {
            for (i = 0; i < 8; i++)
            {
                if ((particle2 = particle->bonds[i]) == NULL) continue;
                bondDisplay.r = bondDisplay.g = bondDisplay.b = 0.0f;
                bondDisplay.repeat = 1;
                bondDisplay.pattern = 0xffff;
                appGetBondDisplay(particle->bondProperties[i], bondDisplay);
                if (bondDisplay.pattern != 0xffff)
                {
                    glEnable(GL_LINE_STIPPLE);
                    glLineStipple(bondDisplay.repeat, bondDisplay.pattern);
                }
                glBegin(GL_LINES);
                glColor3f(bondDisplay.r, bondDisplay.g, bondDisplay.b);
                x2 = CellWidth * particle->vPosition.x;
                y2 = CellHeight * particle->vPosition.y;
                y2 = WindowHeight - y2;
                x3 = CellWidth * particle2->vPosition.x;
                y3 = CellHeight * particle2->vPosition.y;
                y3 = WindowHeight - y3;
                glVertex2f(x2, y2);
                glVertex2f(x3, y3);
                glEnd();
                if (bondDisplay.pattern != 0xffff)
                {
                    glDisable(GL_LINE_STIPPLE);
                }
            }
        }
    }

    // User messages.
    glColor3f(0.0f, 0.0f, 0.0f);
    renderBitmapString(5, 15, FONT, "h for help");
    if (Step)
    {
        renderBitmapString(WINDOW_WIDTH - 60, WINDOW_HEIGHT -  10, FONT, "Step");
    } else if (Pause)
    {
        renderBitmapString(WINDOW_WIDTH - 60, WINDOW_HEIGHT -  10, FONT, "Pause");
    }
    if (Progress)
    {
        sprintf(buf, "Cycle = %d/%d", CycleCount, Cycles);
        renderBitmapString(5, WINDOW_HEIGHT - 10, FONT, buf);
    }

    glutSwapBuffers();
    glFlush();
}


// Draw a particle.
void drawParticle(Particle *particle)
{
    struct ParticleDisplay display;

    // Get application particle display specification.
    display.r = display.g = display.b = 0.0f;
    display.solid = false;
    display.label = '\0';
    appGetParticleDisplay(particle, display);

    // Set particle color.
    glColor3f(display.r, display.g, display.b);

    // Draw particle.
    if (display.solid)
    {
        glBegin(GL_POLYGON);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }
    float x = CellWidth * particle->vPosition.x;
    float y = CellHeight * particle->vPosition.y;
    y = WindowHeight - y;
    float rx = CellWidth * particle->fRadius;
    float ry = CellHeight * particle->fRadius;

    // Draw circle.
    glVertex2f(x + rx, y);
    int sides = 20;
    float ad = 360.0f / (float)sides;
    float a = ad;
    for (int i = 1; i < sides; i++, a += ad)
    {
        glVertex2f(x + (rx * cos(DegreesToRadians(a))),
            y + (ry * sin(DegreesToRadians(a))));
    }
    glEnd();

    // Draw particle label.
    if (display.solid) glColor3f(0.0f, 0.0f, 0.0f);
    if (display.label != '\0')
    {
        char buf[2];
        buf[0] = display.label;
        buf[1] = '\0';
        renderBitmapString(x - (CellWidth / 3.0), y + (CellHeight / 3.0), FONT, buf);
    }
}


// Idle.
void idle()
{
    // Done?
    if (CycleCount >= Cycles || Quit) terminate(0);

    if (UserMode == HELP)
    {
        helpInfo();

    }
    else
    {

        // Run a cycle.
        if (!Pause)
        {
            automaton->step();
            CycleCount++;
        }

        // Reset pause?
        if (Step) Pause = true;

        // Display.
        display();
    }
}


// Reshape window.
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    WindowWidth = w;
    WindowHeight = h;
    CellWidth = (float)WindowWidth / (float)WIDTH;
    CellHeight = (float)WindowHeight / (float)HEIGHT;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, (float)WindowWidth, 0.0f, (float)WindowHeight);
    glScalef(1, -1, 1);
    glTranslatef(0, -WindowHeight, 0);
}


// Keyboard input.
void keyInput(unsigned char key, int x, int y)
{
    if (key == 'q')
    {
        Quit = true;
        return;
    }
    if (key == 'h')
    {
        UserMode = HELP;
        return;
    }
    if (UserMode == HELP)
    {
        UserMode = RUN;
        return;
    }
    if (key == 'c')
    {
        Progress = !Progress;
        return;
    }
    if (key == 'p')
    {
        if (Step)
        {
            Step = false;
            Pause = true;
        }
        else
        {
            Pause = !Pause;
        }
        return;
    }
    if (key == 's')
    {
        Step = !Step;
        if (Step)
        {
            Pause = true;
        }
        else
        {
            Pause = false;
        }
        return;
    }
    if (key == ' ')
    {
        if (Step) Pause = false;
        return;
    }
}


// Help for controls.
void helpInfo()
{
    int i,v;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);

    v = 15;
    renderBitmapString(5, v, FONT, "Controls:"); v += (2 * LINE_SPACE);
    for (i = 0; ControlInfo[i] != NULL; i++)
    {
        renderBitmapString(5, v, FONT, ControlInfo[i]);
        v += LINE_SPACE;
    }

    glutSwapBuffers();
    glFlush();
}


// Print string on screen at specified location.
void renderBitmapString(GLfloat x, GLfloat y, void *font, char *string)
{
    char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}
