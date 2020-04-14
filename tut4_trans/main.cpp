/*
 * robot-arm-2D.c
 *
 * This program shows how to composite modeling transformations
 * to draw translated and rotated hierarchical models.
 * Interaction:  pressing the S/s and E/e keys, alters shoulder and elbow
 * joint rotation angles, and L/l, R/r the gripper plate positions.
 */
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

enum { shoulder, elbow, wrist, left_plate, right_plate, n_joints } Joint;
float robot_arm[n_joints] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
float increment[n_joints] = { 1.0, 1.0, 0.0, 1.0, 1.0 };

void init(void)
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
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


void myWireRect(float l, float h)
{
  glPushMatrix();
    glScalef(l, h, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.5, -0.5);
    glVertex2f(0.5, -0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(-0.5, 0.5);
    glEnd();
  glPopMatrix();
}

void myWireRhombus(float l, float h)
{
  glPushMatrix();
    glScalef(l, h, 1.0);
    glScalef(1.0/sqrt(2.0), 1.0/sqrt(2.0), 1.0);
    glRotatef(45.0, 0.0, 0.0, 1.0);
    myWireRect(1.0, 1.0);
  glPopMatrix();
}

void drawArm(void)
{
  glPushMatrix();

    drawAxes(1.0);
  // Shoulder
  glTranslatef(0.0, 0.0, 0.0);
  glRotatef(robot_arm[shoulder], 0.0, 0.0, 1.0);
    drawAxes(0.2);
  // Upper arm
  glTranslatef(0.2, 0.0, 0.0);
  myWireRhombus(0.4, 0.2);
    drawAxes(0.2);
  // Elbow
  glTranslatef(0.2, 0.0, 0.0);
  glRotatef(robot_arm[elbow], 0.0, 0.0, 1.0);
    drawAxes(0.2);
  // Lower arm
  glTranslatef(0.0, -0.2, 0.0);
  myWireRhombus(0.2, 0.4);
    drawAxes(0.2);
  // Wrist
  glTranslatef(0.0, -0.2, 0.0);
  glRotatef(robot_arm[wrist], 0.0, 0.0, 1.0);
    drawAxes(0.2);
  // Hand
  glTranslatef(0.0, -0.025, 0.0);
  myWireRect(0.2, 0.05);
    drawAxes(0.2);
  // Gripper
  glPushMatrix();
    glTranslatef(-0.075, -0.025, 0.0);
    drawAxes(0.2);
    glRotatef(robot_arm[left_plate], 0, 0, 1);
    glTranslatef(0.0, -0.0625, 0.0);
    
  myWireRect(0.05, 0.125);
    drawAxes(0.2);
  glPopMatrix();
    
  glPushMatrix();
    glTranslatef(0.075, -0.025, 0.0);
      drawAxes(0.2);
      glRotatef(robot_arm[right_plate], 0, 0, 1);
      glTranslatef(0.0, -0.0625, 0.0);
      
    myWireRect(0.05, 0.125);
    drawAxes(0.2);
  glPopMatrix();
  
  glPopMatrix();
}

void display(void)
{
  int err;

  glClear (GL_COLOR_BUFFER_BIT);

  drawArm();
  glutSwapBuffers();

  if ((err = glGetError()) != GL_NO_ERROR) {
    printf("display: %s\n",gluErrorString(err));
  }
}

void reshape (int w, int h)
{
  glViewport (0, 0, (GLsizei) w * 2, (GLsizei) h * 2);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  case 'S':
    robot_arm[shoulder] += increment[shoulder];
    glutPostRedisplay();
    break;
  case 's':
    robot_arm[shoulder] -= increment[shoulder];
    glutPostRedisplay();
    break;
  case 'E':
    robot_arm[elbow] += increment[elbow];
    glutPostRedisplay();
    break;
  case 'e':
    robot_arm[elbow] -= increment[elbow];
    glutPostRedisplay();
    break;
  case 'L':
    robot_arm[left_plate] += increment[left_plate];
    glutPostRedisplay();
    break;
  case 'l':
    robot_arm[left_plate] -= increment[left_plate];
    glutPostRedisplay();
    break;
  case 'R':
    robot_arm[right_plate] += increment[right_plate];
    glutPostRedisplay();
    break;
  case 'r':
    robot_arm[right_plate] -= increment[right_plate];
    glutPostRedisplay();
    break;
  case 27:
    exit(0);
    break;
  default:
    break;
  }
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize (500, 500);
  glutInitWindowPosition (100, 100);
  glutCreateWindow (argv[0]);
  init ();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
