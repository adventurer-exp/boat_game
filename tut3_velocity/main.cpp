//
//  main.cpp
//  tut3_velocity
//
//  Created by Simona Bilkova on 20/3/20.
//  Copyright © 2020 Simona Bilkova. All rights reserved.
//
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
typedef struct { float r, theta; } vec2fPolar;

typedef struct { vec2f r0, v0, r, v; } state;
state projectile = {
    { 0.0, 0.0 },
    { 0.25, 0.25 },
    { 0.0, 0.0 },
    { 0.0, 0.0 }
};

vec2fPolar initVel = {
    1.0,
    60.0
};

/* Use a low gravity value, rather than true Earth gravity of 9.8,
 * to slow down projectile acceleration, otherwise too fast for
 * nice projectile motion aesthetics.
 */
const float g = -0.25;
const int milli = 1000;
const float angle = 1.0;
const float mag = 0.05;

typedef enum { analytical, numerical } integrationMode;

typedef struct {
    bool debug;
    bool go;
    bool tangents;
    bool normals;
    float startTime;
    integrationMode integrateMode;
    bool OSD;
    int frames;
    float frame_rate;
    float frame_rate_interval;
    float last_frame_rate_t;
} global_t;

global_t global = { true, false, false, false, 0.0, numerical, true, 0, 0.0, 0.2, 0.0 };

float degToRad(float deg) {
    return deg * M_PI / 180;
}

float radToDeg(float rad) {
    return rad * 180 / M_PI;
}

void convertPolToCar() {
    projectile.v0.x = initVel.r * 2.5 * cosf(degToRad(initVel.theta));
    printf("x: %.2f\n", projectile.v0.x);
    projectile.v0.y = initVel.r * 2.5 * sinf(degToRad(initVel.theta));
    printf("y: %.2f\n", projectile.v0.y);
    
    projectile.r0.x = initVel.r * cosf(degToRad(initVel.theta));
    projectile.r0.y = initVel.r  *cosf(degToRad(initVel.theta));
}

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
        printf("r x: %f, r y: %f\nv x: %f, v y: %f\n", projectile.r.x, projectile.r.y, projectile.v.x,  projectile.v.y);
    
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
//            updateProjectileStateAnalytical(t);
            updateProjectileStateNumerical(dt);
    } else {
        if (global.go) {
            global.go = false;
//            convertPolToCar();
        }
    }
}


void displayProjectile(void)
{
    float r = 0.05;
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(projectile.r.x, projectile.r.y, 0);
    glutWireSphere(r, 10, 5);
    glPopMatrix();
}

void displayVector()
{
    convertPolToCar();
    glBegin(GL_LINES);
    glColor3f(0.75, 0.75, 0.75);
    glVertex2f(projectile.r.x, projectile.r.y);
    glVertex2f(projectile.v0.x, projectile.v0.y);
    glEnd();
}

void drawVector(float x, float y, float a, float b, float s, bool normalize, float red, float green, float blue) {
    float z = 0;
    
    glBegin(GL_LINES);
    glColor3f(red, green, blue);

//    float length = sqrt(a * a + b * b);
    glVertex3f(x, y, z);
    if (normalize) {
        float magnitude = sqrt(a * a + b * b);
        glVertex3f(x + a / magnitude * s, y + b / magnitude * s, z);
    } else {
        glVertex3f(x + a * s, y + b * s, z);
    }
    
    /*
    glVertex3f(x, y, z);
    printf("Draw vector beginning: x: %f y: %f \n", x, y);
    
    if (normalize) {
        float magnitude = sqrt(a * a + b * b);
        a = a / magnitude;
        glVertex3f(x + a / magnitude, y + b / magnitude, z);
        printf("Draw vector: x: %f y: %f \n", x + a, y + b);
    } else {
        glVertex3f(x + a * s, y + b * s, z);
    }
     */
    
    glColor3f(1, 1, 1);
    glEnd();
    
}

float calculateY(float t) {
    return 1.0 / 2.0 * g * t * t + projectile.v0.y * t + projectile.r.y;
}

void drawTrajectoryAnalytical()
{
    float t = (2 * projectile.v0.y) / -g;
    float n = 15.0;
    float left = projectile.r.x;
    float right = projectile.r.x + projectile.v0.x * t;
    float range = right - left;
    float stepSize = range / n;
    printf("left: %.2f, right: %.2f, range: %.2f, TOF: %.2f, stepsize: %.2f\n", left, right, range, t, stepSize);
    
    float x = 0;
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 0.862745, 0.294117);
    
    float myT = 0.0;
    float y = 0.0;
    for(int i = 0; i <= n; i++) {
        x = left + stepSize * i;
        myT = t / n * i;
        y = calculateY(myT);
        printf("y: %.2f, at time: %.2f, first part: %.2f\n", y, myT, 1.0 / 2.0 * g * myT * myT);
        glVertex3f(x, y, 0);
    }
    glEnd();
    
    if (global.tangents || global.normals) {
        for(int i = 0; i <= n; i++) {
            x = left + stepSize * i;
            myT = t / n * i;
            y = calculateY(myT);
            if(global.tangents)
                drawVector(x, y, projectile.v0.x, g * myT + projectile.v0.y, 0.1, true, 1, 0, 0);
            if (global.normals)
                drawVector(x, y, -(g * myT + projectile.v0.y), projectile.v0.x, 0.1, true, 0, 1, 0);
        }
    }
}

void drawTrajectoryNumerical()
{
    float left = projectile.r.x;
    
    float x = 0;
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 0.862745, 0.294117);
    
    float y = 0.0;
    float myT = 0.0;
    const float dt = 0.1;
    while (y >= 0.0)
    {
        myT += dt;
        x = left + projectile.v0.x * myT;
        y = 1.0 / 2.0 * g * myT * myT + projectile.v0.y * myT + projectile.r.y;
        printf("y: %.2f, at time: %.2f, first part: %.2f\n", y, myT, 1.0 / 2.0 * g * myT * myT);
        glVertex3f(x, y, 0);
    }
    
    glEnd();
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
    if (!global.go) {
        displayVector();
//        drawTrajectoryAnalytical();
        drawTrajectoryNumerical();
    }
    
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
        case 'h':
            global.debug = !global.debug;
            break;
        case 'i':
            if (global.integrateMode == analytical)
                global.integrateMode = numerical;
            else
                global.integrateMode = analytical;
            break;
        case 't':
            global.tangents = !global.tangents;
            break;
        case 'n':
            global.normals = !global.normals;
            break;
        case 'a':
            initVel.theta += angle;
            printf("Angle: %.2f\n", initVel.theta);
            convertPolToCar();
            break;
        case 'd':
            initVel.theta -= angle;
            printf("Angle: %.2f\n", initVel.theta);
            convertPolToCar();
            break;
        case 'w':
            initVel.r += mag;
            printf("Magnitude: %.2f\n", initVel.r);
            convertPolToCar();
            break;
        case 's':
            initVel.r -= mag;
            printf("Magnitude: %.2f\n", initVel.r);
            convertPolToCar();
            break;
        case 'o':
            global.OSD = !global.OSD;
            break;
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

