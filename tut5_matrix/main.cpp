#define GL_SILENCE_DEPRECATION

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
/* Example showing transormation composition */

typedef float Array4X4f[4][4];
typedef float Vec16f[16];

bool traceMVMatrix = true;

void drawAxes(float length)
{

  glPushAttrib(GL_CURRENT_BIT);
  glBegin(GL_LINES);

  /* x axis */
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(length, 0.0, 0.0);

  /* y axis */
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, length, 0.0);

  /* z axis */
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, length);

  glEnd();
  glPopAttrib();
}

void drawGrid2D(float length, float step)
{
  float x, y;

  glPushAttrib(GL_CURRENT_BIT);
  glColor3f(1.0, 1.0, 1.0);

  /* x axis */
  glBegin(GL_LINES);
  for (x = 0.0; x <= length; x += step) {
    glVertex2f(x, 0.0);
    glVertex2f(x, length);
  }
  glEnd();

#if 0
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0xF0F0);
  glBegin(GL_LINES);
  for (x = 0.0 + step / 2.0; x <= length; x += step) {
    glVertex2f(x, 0.0);
    glVertex2f(x, length);
  }
  glEnd();
  glDisable(GL_LINE_STIPPLE);
#endif

  /* y axis */
  glBegin(GL_LINES);
  for (y = 0.0; y <= length; y += step) {
    glVertex2f(0.0, y);
    glVertex2f(length, y);
  }
  glEnd();

#if 0
  glEnable(GL_LINE_STIPPLE);
  glBegin(GL_LINES);
  for (y = 0.0 + step / 2.0; y <= length; y += step) {
    glVertex2f(0.0, y);
    glVertex2f(length, y);
  }
  glEnd();
  glDisable(GL_LINE_STIPPLE);
#endif

  glPopAttrib();
}

/*
 * Transform and draw a teapot.
 */
void drawTeapot()
{
  drawAxes(2.0);
  glutWireTeapot(1.0);      /* Teapot */
}

void drawUnitRectangle()
{
  drawAxes(2.0);

  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_QUADS);
  glVertex2f(0.0, 0.0);
  glVertex2f(1.0, 0.0);
  glVertex2f(1.0, 1.0);
  glVertex2f(0.0, 1.0);
  glEnd();
}
  
void getAndPrintMVMatrixRowMajor()
{
  int i, j;
  float m[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  for (i = 0; i < 16; i++)
    printf("%4.2f ", m[i]);
  printf("\n");
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++)
      printf("%5.2f ", m[j*4+i]);
    printf("\n");
  }
  printf("\n");
}

void getAndPrintMVMatrixColumnMajor()
{
  int i, j;
  float m[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  for (i = 0; i < 16; i++)
    printf("%4.2f ", m[i]);
  printf("\n");
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++)
      printf("%5.2f ", m[i*4+j]);
    printf("\n");
  }
  printf("\n");
}


void display(void)
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  glLoadIdentity();

  if (traceMVMatrix) {
    printf("%s %d\n", __FILE__, __LINE__);
    getAndPrintMVMatrixRowMajor();
  }

  drawGrid2D(5.0, 1.0);

  drawAxes(5.0);

  //glTranslatef(4.0, 0.0, 0.0);
  if (traceMVMatrix) {
    printf("%s %d\n", __FILE__, __LINE__);
    getAndPrintMVMatrixRowMajor();
  }

#if 0
  glTranslatef(3.0, 0.0, 0.0);
  glRotatef(30.0, 0.0, 0.0, 1.0);
#endif

  if (traceMVMatrix) {
    printf("%s %d\n", __FILE__, __LINE__);
    getAndPrintMVMatrixRowMajor();
  }

#if 0
  drawTeapot();
#endif
  glColor3f(1.0, 1.0, 0.0);

  drawUnitRectangle();

  glColor3f(1.0, 1.0, 1.0);

  glPopMatrix();

  glutSwapBuffers();

  if ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));
}

void key(unsigned char k, int x, int y)
{
  printf("key = %u\n", k);

  switch (k) {
  case 27:  /* Escape */
    exit(0);
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

void myReshape(int w, int h)
{
  glViewport(0, 0, w *2 , h *2);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (w <= h)
    glOrtho(-1.0, 6.0, -1.0, 6.0 * (GLfloat) h / (GLfloat) w, -5.0, 5.0);
  else
    glOrtho(-1.0, 6.0 * (GLfloat) w / (GLfloat) h, -1.0, 6.0, -5.0, 5.0);

#if 0
  gluPerspective(90.0, w / (GLfloat) h, 0.1, 1000.0);
#endif
  glMatrixMode(GL_MODELVIEW);
}


/*
 * Main Loop Open window with initial window size, title bar, RGBA display
 * mode, and handle input events.
 */
int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 50);
  glutInitWindowSize(400, 400);
  glutCreateWindow(argv[0]);
  glutReshapeFunc(myReshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  glutMainLoop();

   return 0;
}
