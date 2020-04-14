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
    { 0.0, 0.0 },
    { 0.0, 0.0 },
    { 0.0, 0.0 }
};

vec2fPolar canon = {
    0.5,
    0.0
};

float platform_angle = 0;

const float g = -0.25;


float degToRad(float deg) {
    return deg * M_PI / 180;
}

float radToDeg(float rad) {
    return rad * 180 / M_PI;
}

float time = 0;
void drawTrajectoryNumerical()
{
    float left = projectile.r.x;
    
    float x = 0;
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 0.862745, 0.294117);
    
    float y = 0.0;
    float myT = time;
    float end_t = 0.0;
    const float dt = 0.1;
    while (y >= 0.0)
    {
        myT += dt;
        x = left + projectile.v0.x * myT;
        y = 1.0 / 2.0 * g * myT * myT + projectile.v0.y * myT + projectile.r.y;
        printf("y: %.2f, at time: %.2f, first part: %.2f\n", y, myT, 1.0 / 2.0 * g * myT * myT);
        glVertex3f(x, y, 0);
        end_t += dt;
    }
    if (time >= end_t)
        time = 0;
    else
        time += dt;
    printf("time: %.2f\n", time);
    glEnd();
}


void convertPolToCar() {
    projectile.r0.x = canon.r * cosf(degToRad(canon.theta + platform_angle));
    printf("r x: %.2f\n", projectile.v0.x);
    projectile.r0.y = canon.r * sinf(degToRad(canon.theta+ platform_angle));
    printf("r y: %.2f\n", projectile.v0.y);
    
    projectile.v0.x =  cosf(degToRad(canon.theta+ platform_angle));
    printf("v x: %.2f\n", projectile.v0.x);
    projectile.v0.y =  sinf(degToRad(canon.theta+ platform_angle));
    printf("v y: %.2f\n", projectile.v0.y);
    
    printf("angle %.2f\n", canon.theta);
}

void updateProjectileStateNumerical(float dt)
{
    // Euler integration
    
    // Position
    projectile.r.x += projectile.v.x * dt;
    projectile.r.y += projectile.v.y * dt;
     
    // Velocity
    projectile.v.y += g * dt;
}

void drawAxes(float l)
{
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(l, 0.0, 0.0);
    glColor3f(0, 1, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, l, 0.0);
    glColor3f(0, 0, 1);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, l);
    glEnd();
    glColor3f(1, 1, 1);
}

void drawProjectile(){
    glColor3f(1.0, 1.0, 1.0);
    float x = 0.01 * cosf(degToRad(60));
        float y = 0.01 * sinf(degToRad(60));
        glPushMatrix();
    glScalef(5.0, 5.0, 5.0);
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(projectile.r.x, projectile.r.y, 0);
    //    printf("r x: %.2f, r y: %.2f\n", projectile.r.x, projectile.r.y);
        glVertex3f(0.0, 0.0, 0.01);
        glVertex3f(0.01, 0.0, 0.0);
        glVertex3f(x, y, 0.0);
        glVertex3f(-x, y, 0.0);
        glVertex3f(-0.01, 0.0, 0.0);
        glVertex3f(-x, -y, 0.0);
        glVertex3f(x, -y, 0.0);
    glVertex3f(0.01, 0.0, 0.0);
        glEnd();

    glPopMatrix();
}

void drawLine(){
    glBegin(GL_LINES);
    glVertex3f(-1.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();
}

void drawCanon(){
    glPushMatrix();
    glRotatef(platform_angle, 0.0, 0.0, 1.0);
    drawLine();
    
    glRotatef(canon.theta, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0, -0.05, 0.0);
    glVertex3f(0.0, 0.05, 0.0);
    glVertex3f(canon.r, 0.05, 0.0);
    glVertex3f(canon.r, -0.05, 0.0);
    glEnd();
    glPopMatrix();
    
}

float calculateY(float t) {
    return 1.0 / 2.0 * g * t * t + projectile.v0.y * t + projectile.r.y;
}

void display(void)
{
    GLenum err;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawAxes(1.0);
    
    drawCanon();
    glPopMatrix();
    drawProjectile();
    drawTrajectoryNumerical();
    glutSwapBuffers();
    
    // Check for errors
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("%s\n",gluErrorString(err));
}

void keyboardCB(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'a':
            canon.theta -= 1;
            convertPolToCar();
            break;
        case 'A':
            canon.theta += 1;
            convertPolToCar();
        break;
        case 'd':
            platform_angle -= 1;
            convertPolToCar();
            break;
        case 'D':
            platform_angle += 1;
            convertPolToCar();
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

// Idle callback for animation
void update(void)
{
    static float lastT = -1.0;
    float t, dt;
    
    
    t = glutGet(GLUT_ELAPSED_TIME);
    t /= 5000.0;
    
    if (lastT < 0.0) {
        lastT = t;
        return;
    }
    dt = t - lastT;
 
    lastT = t;
    
    if ((projectile.r.y == projectile.r0.y) ||
            projectile.r.y + projectile.v.y * dt >= 0 )
        {
            updateProjectileStateNumerical(dt);
        }
    glutPostRedisplay();
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

