//
//  main.cpp
//  tut3_anim
//
//  Created by Simona Bilkova on 20/3/20.
//  Copyright © 2020 Simona Bilkova. All rights reserved.
//


/*
 * projectile-motion.c
 *
 * This program shows how to compute projectile motion using both
 * analytical and numerical integration of the equations of motion.
 *
 * $Id: projectile-motion.c,v 1.9 2019/03/22 02:53:51 gl Exp gl $
 *
 */
#define GL_SILENCE_DEPRECATION

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

typedef struct { float x, y; } vec2f;

typedef struct { vec2f r0, v0, r, v; } state;
state projectile = {
    { 0.0, 0.0 },
    { 0.25, 0.25 },
    { 0.0, 0.0 },
    { 0.0, 0.0 }
};

/* Use a low gravity value, rather than true Earth gravity of 9.8,
 * to slow down projectile acceleration, otherwise too fast for
 * nice projectile motion aesthetics.
 */
const float g = -0.25;
const int milli = 1000;

typedef enum { analytical, numerical } integrationMode;

typedef struct {
    bool debug;
    bool go;
    float startTime;
    integrationMode integrateMode;
    bool OSD;
    int frames;
    float frame_rate;
    float frame_rate_interval;
    float last_frame_rate_t;
} global_t;

global_t global = { true, false, 0.0, numerical, true, 0, 0.0, 0.2, 0.0 };

void updateProjectileStateAnalytical(float t)
{
    vec2f r0, v0;
    
    r0 = projectile.r0;
    v0 = projectile.v0;
    
    projectile.r.x = v0.x * t + r0.x;
    projectile.r.y = 1.0 / 2.0 * g * t * t + v0.y * t + r0.y;
    /*
     * The above statement can alternatively be written as below, saving
     * a few arithmetic operations, which may or may not be worthwhile
     * and which can obscure code and introduce hard to find bugs:
     *
     * 1) Precalculate 1.0 / 2.0 as 0.5 to save a division
     * projectileState.r.y = 0.5 * g * t * t + v0.y * t + r0.y;
     *
     * 2) Factorise to save a multiplication
     * projectileState.r.y = t * (0.5 * g * t + v0.y) + r0.y;
     */
}

void updateProjectileStateNumerical(float dt)
{
    // Euler integration
    
    // Position
    projectile.r.x += projectile.v.x * dt;
    projectile.r.y += projectile.v.y * dt;
    if (global.debug)
        printf("r x: %f2, r y: %f2\nv x: %f2, v y: %f2\n", projectile.r.x, projectile.r.y, projectile.v.x,  projectile.v.y);
    
    // Velocity
    projectile.v.y += g * dt;
    
}

void updateProjectileState(float t, float dt)
{
    if (global.debug)
        printf("global.integrateMode: %d\n", global.integrateMode);
    if ((global.go && projectile.r.y == projectile.r0.y) ||
        projectile.r.y + projectile.v.y * dt >= 0 )
    {
        if (global.integrateMode == analytical)
            updateProjectileStateAnalytical(t);
        else
            updateProjectileStateNumerical(dt);
    }
}


void displayProjectile(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(projectile.r.x, projectile.r.y, 0);
    glutWireSphere(0.05, 10, 5);
    glPopMatrix();
    
#if 0
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glVertex2f(projectile.r.x, projectile.r.y);
    glEnd();
#endif
}

// Idle callback for animation
void update(void)
{
    static float lastT = -1.0;
    float t, dt;
    
    if (!global.go)
        return;
    
    t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;
    
    if (lastT < 0.0) {
        lastT = t;
        return;
    }
    
    dt = t - lastT;
    if (global.debug)
        printf("%f %f\n", t, dt);
    updateProjectileState(t, dt);
    lastT = t;
    
    /* Frame rate */
    dt = t - global.last_frame_rate_t;
    if (dt > global.frame_rate_interval) {
        global.frame_rate = global.frames / dt;
        global.last_frame_rate_t = t;
        global.frames = 0;
    }
    
    glutPostRedisplay();
}

void displayOSD()
{
    char buffer[30];
    char *bufp;
    int w, h;
    
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    /* Set up orthographic coordinate system to match the
     window, i.e. (0,0)-(w,h) */
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* Frame rate */
    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(10, 60);
    snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", global.frame_rate);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    
    /* Time per frame */
    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(10, 40);
    snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / global.frame_rate * 1000.0);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    
    /* Pop modelview */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    
    /* Pop projection */
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* Pop attributes */
    glPopAttrib();
}

void display(void)
{
    GLenum err;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex2f(0, 0);
    glVertex2f(1, 0);
    glColor3f(0, 1, 0);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glEnd();
    
    glColor3f (0.8, 0.8, 0.8);
    
    // Display projectile
    displayProjectile();
    
    // Display OSD
    if (global.OSD)
        displayOSD();
    
    glPopMatrix();
    
    glutSwapBuffers();
    
    global.frames++;
    
    // Check for errors
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("%s\n",gluErrorString(err));
}


void myinit (void)
{
}

void keyboardCB(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'd':
            global.debug = !global.debug;
            break;
        case 'i':
            if (global.integrateMode == analytical)
                global.integrateMode = numerical;
            else
                global.integrateMode = analytical;
            break;
        case 'o':
            global.OSD = !global.OSD;
            break;
        case 's':
        case ' ':
            if (!global.go) {
                global.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
                global.go = true;
                projectile.v.x = projectile.v0.x;
                projectile.v.y = projectile.v0.y;
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w *2, h*2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(500, 500);
    glutCreateWindow("Projectile Motion");
    
    glutKeyboardFunc(keyboardCB);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutIdleFunc(update);
    
    glutMainLoop();
}
