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
/* Use a low gravity value, rather than true Earth gravity of 9.8,
 * to slow down projectile acceleration, otherwise too fast for
 * nice projectile motion aesthetics.
 */

const int n = 15;
const float r = 0.05;
bool keyPressed = false;

void drawCircle()
{
    float x, y;
    float theta;

    glPointSize(2.0);
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i <= n; i++) {
      theta = i / (float)n * 2.0 * M_PI;
      x = r * cosf(theta);
      y = r * sinf(theta);
      glVertex2f(x, y);
    }
    glEnd();
}

// Idle callback for animation
void update(void)
{
    if (keyPressed) {
        printf("K pressed\n");
    }
    glutPostRedisplay();
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
    drawCircle();
    
    glColor3f (0.8, 0.8, 0.8);
    
    
    glPopMatrix();
    
    glutSwapBuffers();
    
    
    
    // Check for errors
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("%s\n",gluErrorString(err));
}


void myinit (void)
{
}

void keyUp(unsigned char key, int x, int y)
{
    switch (key) {
        case 'k':
            keyPressed = false;
            break;
        default:
            break;
    }
    printf("Key Up %c", key);
}

void keyboardCB(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'k':
            keyPressed = true;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w * 2, h * 2);
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
    glutKeyboardUpFunc(keyUp);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutIdleFunc(update);
    
    glutMainLoop();
}

