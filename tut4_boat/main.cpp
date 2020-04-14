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

float cannon_angle = 45.0, boat_rotation = 0;
const float angle_d = 1.0;

// Idle callback for animation
void update(void)
{
    glutPostRedisplay();
}

float degToRad(float deg) {
    return deg * M_PI / 180;
}

float radToDeg(float rad) {
    return rad * 180 / M_PI;
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

void drawSquare()
{
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.25, -0.25, 0.0);
    glVertex3f(0.25, -0.25, 0.0);
    glVertex3f(0.25, 0.25, 0.0);
    glVertex3f(-0.25, 0.25, 0.0);
    glEnd();
}

float calculateX()
{
    return cosf(degToRad(cannon_angle)) * 0.5;
}

float calculateY()
{
    return sinf(degToRad(cannon_angle)) * 0.5;
}

void calculateProjectileStart()
{
//    to be used as r x and ry as a starting point of the projectile
    float x, y;
    x = calculateX() + 0.5;
    y = calculateY() + 0.25;
    printf("x: %.2f, y: %.2f\n", x, y);
}

void drawTrapezoid(){
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(-0.5, -0.25, 0.0);
    glVertex3f(0.5, -0.25, 0.0);
    glVertex3f(1, 0.25, 0.0);
    glVertex3f(-1.0, 0.25, 0.0);
    glEnd();
}

void drawBoat()
{
    glRotatef(boat_rotation, 0.0, 0.0, 1.0);
    glPushMatrix();
    
//    main deck
    glPushMatrix();

    drawAxes(0.2);
    glPopMatrix();
    
//    bridge
    glPushMatrix();
    glTranslatef(0.0, 0.5, 0.0);
    drawAxes(0.2);
    drawSquare();
    glPopMatrix();
    
//    cannon
    glPushMatrix();
    glTranslatef(0.5, 0.25, 0.0);
    glRotatef(cannon_angle, 0.0, 0.0, 1.0);
    drawAxes(0.2);
    glTranslatef(0.25, 0.0, 0.0);
    drawAxes(0.2);
    glScalef(1.0, 0.25, 1.0);
    drawSquare();
    glTranslatef(0.25, 0.0, 0.0);
    drawAxes(0.2);
    glPopMatrix();
    
    glPopMatrix();
}

void display(void)
{
    GLenum err;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawAxes(1.0);
    drawBoat();
    calculateProjectileStart();
    
    glPopMatrix();
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
        case 'c':
            cannon_angle -= angle_d;
            break;
        case 'C':
            cannon_angle += angle_d;
            break;
        case 'b':
            boat_rotation -= angle_d;
            break;
        case 'B':
            boat_rotation += angle_d;
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

