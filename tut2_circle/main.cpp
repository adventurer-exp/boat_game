/* Draws a circle using either cartesian or polar coordinates */

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#if _WIN32
#   include <Windows.h>
#endif
#if __APPLE__
#define GL_SILENCE_DEPRECATION
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum { cartesian, parametric } coordSystem = { parametric };
enum { points, lines } renderMode = { lines };

bool debug = true;

int n = 32;

// Utility function for drawing axes
void drawAxes(float len)
{
#if 0
    glPushAttrib(GL_CURRENT_BIT);
#endif
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(len, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, len, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, len);
    glEnd();
#if 0
    glPopAttrib();
#endif
}

// Cartesian coordinates
void drawCircleCartesian(float r, int n)
{
    float x, y;
    
    if (debug)
        printf("drawCircleCartesian: %d\n", n);
    
    glColor3f(1.0, 1.0, 1.0);
    if (renderMode == points) {
        glPointSize(2.0);
        glBegin(GL_POINTS);
    } else {
        glLineWidth(2.0);
        glBegin(GL_LINE_LOOP);
    }
    
    n /= 2;
    
    for (int i = 0; i < n; i++) {
        x = ((i / (float)n - 0.5) * 2.0) * r;
        y = sqrt(r * r - x * x);
        if (debug)
            printf("x y: %f %f\n", x, y);
        glVertex2f(x, y);
        if (renderMode == points) {
            y = -y;
            if (debug)
                printf("x y: %f %f\n", x, y);
            glVertex2f(x, y);
        }
    }
    
    if (renderMode == lines)
        for (int i = n; i > 0; i--) {
            x = (i / (float)n - 0.5) * 2.0 * r;
            y = -sqrt(r * r - x * x);
            if (debug)
                printf("x y: %f %f\n", x, y);
            glVertex2f(x, y);
        }
    
    glEnd();
    
#if 0
    // Ignore
#endif
}

// Parametric coordinates
void drawCircleParametric(float r, int n)
{
    float x, y;
    float theta;
    
    if (debug)
        printf("drawCircleParametric: %d\n", n);
    
    glColor3f(1.0, 1.0, 1.0);
    if (renderMode == points) {
        glPointSize(2.0);
        glBegin(GL_POINTS);
    } else {
        glLineWidth(2.0);
        glBegin(GL_LINE_LOOP);
    }
    for (int i = 0; i < n; i++) {
        theta = i / (float)n * 2.0 * M_PI;
        x = r * cosf(theta);
        y = r * sinf(theta);
        if (debug)
            printf("x y: %f %f", x, y);
        glVertex2f(x, y);
    }
    glEnd();
}

float calculateY(float x) {
    float y = 0, wavelength = 2.0, amplitude = 1.0;
    float k = 2.0 * M_PI / wavelength;
    
    return y = amplitude * sin(k * x);
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


float calculateYPar(float x) {
    float y = 0;
    return y = 2 * (x * x) + 2 * x + 1;
}


// Display callback
void display()
{
    int err;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawAxes(1.0);
    
    float left = -1.0;
    float right = 1.0;
    float range = right - left;
    float stepSize = range / n;

    float x = 0;
    float y = 0;
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    
    for(int i = 0; i <= n; i++) {
        x = i * stepSize + left;
        y = calculateY(x);
        glVertex3f(x, y, 0);
    }
    glEnd();
    
    for(int i = 0; i <= n; i++) {
        x = i * stepSize + left;
        y = calculateY(x);
        drawVector(x, y, 1.0, cos(x * M_PI) * M_PI, 0.1, true, 1, 0, 0);
        drawVector(x, y, -cos(x * M_PI) * M_PI, 1.0, 0.1, true, 0, 1, 0);
    }

    
#if 0
    glBegin(GL_LINES);
    for(int i = 0; i <= n; i++) {
        x = i * stepSize + left;
        y = calculateY(x);
        drawVector(x, y, 1, cos(x), 0.1, false, 0, 1, 0);
    }
    glEnd();
    
//    Draws a simple line as necessary
    float x = 1.0;
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    glVertex2f(x, -x / 2 );
    glVertex2f(-x, -(-x / 2));
    glEnd();
    
//    Draws circles
    if (coordSystem == cartesian)
    drawCircleCartesian(1.0, n);
    else if (coordSystem == parametric)
    drawCircleParametric(1.0, n);
    
//    Draws parabolic function
    float left = -1.0;
    float right = 1.0;
    float range = right - left;
    float stepSize = range / n;
    
    float x = 0;
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    
    for(int i = 0; i <= n; i++) {
        x = i * stepSize + left;
        glVertex3f(x, calculateY(x), 0);
    }
    glEnd();
    
    float calculateY(float x) {
        float y = 0;
        return y = 2 * (x * x) + 2 * x + 1;
    }
    
//    Draws a sinusoida
    float left = -1.0;
    float right = 1.0;
    float range = right - left;
    float stepSize = range / n;
    
    float x = 0;
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    
    for(int i = 0; i <= n; i++) {
        x = i * stepSize + left;
        glVertex3f(x, calculateY(x), 0);
    }
    glEnd();
    
    float calculateY(float x) {
        float y = 0, wavelength = 0.5, amplitude = 1.0;
        float k = 2.0 * M_PI / wavelength;
        
        return y = amplitude * sin(k * x);
    }
    
    
#endif
    
    // Always check for gl errors
    if ((err = glGetError()) != GL_NO_ERROR)
        fprintf(stderr, "Error: %s\n", gluErrorString(err));
    
    glutSwapBuffers();
}

// Keyboard callback
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'c':
            if (coordSystem == cartesian)
                coordSystem = parametric;
            else
                coordSystem = cartesian;
            break;
        case 'm':
            if (renderMode == lines)
                renderMode = points;
            else
                renderMode = lines;
            break;
        case 'd':
            debug = !debug;
            break;
        case '+':
            n *= 2;
            break;
        case '-':
            n /= 2;
            if (n < 2)
                n = 2;
            break;
        case 27:
            // ESCAPE key
            exit(0);
            break;
    }
    glutPostRedisplay();
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Circle");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
}
