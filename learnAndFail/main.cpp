#include <stdlib.h>
#include <stdio.h>

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

int width;
int height;

void init()
{
    /* In this program these OpenGL calls only need to be done once,
     but normally they would go elsewhere, e.g. display */
    
    glViewport(0, 0, width * 2, height * 2);
    
    glMatrixMode(GL_PROJECTION);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    //glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void reshape(int w, int h)
{
    width = w;
    height = h;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex2f(0, 0);
    glVertex2f(1, 0);
    glColor3f(0, 1, 0);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glEnd();
    
    /* Put drawing code here */
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glEnd();
    
    /*
     glBegin(GL_LINE_LOOP);
     glVertex3f(-0.5, -0.5, -0.5);
     glVertex3f(0.5, -0.5, -0.5);
     
     glVertex3f(0.5, -0.5, -0.5);
     glVertex3f(0.5, 0.5, -0.5);
     
     glVertex3f(0.5, 0.5, -0.5);
     glVertex3f(-0.5, 0.5, -0.5);
     
     glVertex3f(-0.5, 0.5, -0.5);
     glVertex3f(-0.5, -0.5, -0.5);
     glEnd();
     */
    
    glColor3f(1.0f, 0.0f, 0.0f);
    
    glBegin(GL_QUADS);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glEnd();
    
    glColor3f(0.0f, 1.0f, 0.0f);
    
    glBegin(GL_QUADS);
    glVertex3f(-0.25, -0.25, -0.75);
    glVertex3f(0.75, -0.25, -0.75);
    glVertex3f(0.75, 0.75, -0.75);
    glVertex3f(-0.25, 0.75, -0.75);
    glEnd();
    
    /* Always check for errors! */
    int err;
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("display: %s\n", gluErrorString(err));
    
    glutSwapBuffers();
}

void idle()
{
    glutPostRedisplay();
}

/* You can ignore this for now, it just lets you exit when you press 'q' or ESC */
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}

int main(int argc, char **argv)
{
    width = 500;
    height = 500;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Tutorial 1");
    
    init();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}
