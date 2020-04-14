//
//  main.cpp
//  assignment_1
//
//  Created by Simona Bilkova on 3/4/20.
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
#include <iostream>
#include <vector>
#include <map>

typedef struct { float x, y; } vec2f;
typedef struct { float r, g, b; } vec3f;
typedef struct { float r, theta; int health; int angle_d; } vec2fPolar;
typedef struct { vec2f r0, v0, r, v; } state;
typedef struct { bool blue, red, island; } new_projectile_t;

typedef struct {
    bool active;
    float r, start_time;
} defence_t;

typedef struct {
    vec2f position;
    float canon_angle;
    float cannon_length;
    float rotation;
    int movement;
    int angle_d;
    int health;
    state projectile;
} boat_t;

typedef struct {
    bool debug;
    bool tangents;
    bool normals;
    bool wireframe;
    bool animation;
    bool axes;
    bool playing;
    float animationX;
    float g;
    int n;
    int frames;
    float last_frame_rate_t, frame_rate_interval, frame_rate;
    float dt;
} global_t;

vec2fPolar island = {
    0.25,
    30.0,
    100,
    0
};

vec3f red = { 1.0, 0.4, 0.4 };
vec3f blue = { 0.0, 0.5, 1.0 };
vec3f green = { 0.7, 1.0, 0.4 };
vec3f white = { 1.0, 1.0, 1.0 };
vec3f pink = { 1.0, 0.6, 0.8 };
vec3f orange = { 1.0, 0.6, 0.8 };

defence_t blue_defence { false, 0.05, 0.0 };
defence_t red_defence { false, 0.05, 0.0 };

global_t global = { true, true, true, false, true, true, true, 0.0, -0.25, 32, 0, 0.0, 0.2, 0.0, 0.0 };

new_projectile_t new_projectile = { false, false, false };

static float t_last = 0.0, delta_t, blue_time_f = 0.0, red_time_f = 0.0, island_time_f = 0.0;
const int LEFT = -1, RIGHT = 1, STILL = 0, BLUE = 1, RED = 0, INCREASE = 1, DECREASE = -1, NO_MOVE = 0;
const float TIME_APP = 10.0, ANGLE_STEP = 1.0, ISLAND_BORDERS = 0.25;

state* island_projectile;
state* blue_defence_projectile;
state* red_defence_projectile;

std::vector<boat_t*> boats;

std::vector<state*> blue_projectiles;
std::vector<state*> red_projectiles;
std::vector<state*> island_projectiles;

std::map<state*, float> projectile_start_times;

//std::vector<state*> all_projectiles[] = { blue_projectiles, red_projectiles, island_projectiles };

void init(){
    boat_t* red_boat = new boat_t();
    red_boat->position.x = 0.5;
    red_boat->position.y = 0.0;
    red_boat->canon_angle = 45.0;
    red_boat->cannon_length = 0.5;
    red_boat->rotation = 0.0;
    red_boat->movement = 0;
    red_boat->angle_d = 0;
    red_boat->health = 10;
    red_boat->projectile.r0.x = 0.0;
    red_boat->projectile.r0.y = 0.0;
    red_boat->projectile.v0.x = 0.0;
    red_boat->projectile.v0.y = 0.0;
    red_boat->projectile.r.x = 0.0;
    red_boat->projectile.r.y = 0.0;
    red_boat->projectile.v.x = 0.0;
    red_boat->projectile.v.y = 0.0;
    boats.push_back(red_boat);
    
    boat_t* blue_boat = new boat_t();
    blue_boat->position.x = -0.5;
    blue_boat->position.y = 0.0;
    blue_boat->canon_angle = 45.0;
    blue_boat->cannon_length = 0.5;
    blue_boat->rotation = 0.0;
    blue_boat->movement = 0;
    blue_boat->angle_d = 0;
    blue_boat->health = 10;
    blue_boat->projectile.r0.x = 0.0;
    blue_boat->projectile.r0.y = 0.0;
    blue_boat->projectile.v0.x = 0.0;
    blue_boat->projectile.v0.y = 0.0;
    blue_boat->projectile.r.x = 0.0;
    blue_boat->projectile.r.y = 0.0;
    blue_boat->projectile.v.x = 0.0;
    blue_boat->projectile.v.y = 0.0;
    boats.push_back(blue_boat);
    
    island_projectile = new state();
    island_projectile->r0.x = 0.0;
    island_projectile->r0.y = 0.0;
    island_projectile->v0.x = 0.0;
    island_projectile->v0.y = 0.0;
    island_projectile->r.x = 0.0;
    island_projectile->r.y = 0.0;
    island_projectile->v.x = 0.0;
    island_projectile->v.y = 0.0;
    
    blue_defence_projectile = new state();
    blue_defence_projectile->r0.x = 0.0;
    blue_defence_projectile->r0.y = 0.0;
    blue_defence_projectile->v0.x = 0.0;
    blue_defence_projectile->v0.y = 0.0;
    blue_defence_projectile->r.x = 0.0;
    blue_defence_projectile->r.y = 0.0;
    blue_defence_projectile->v.x = 0.0;
    blue_defence_projectile->v.y = 0.0;
    
    red_defence_projectile = new state();
    red_defence_projectile->r0.x = 0.0;
    red_defence_projectile->r0.y = 0.0;
    red_defence_projectile->v0.x = 0.0;
    red_defence_projectile->v0.y = 0.0;
    red_defence_projectile->r.x = 0.0;
    red_defence_projectile->r.y = 0.0;
    red_defence_projectile->v.x = 0.0;
    red_defence_projectile->v.y = 0.0;
}

float degToRad(float deg) {
    return deg * M_PI / 180;
}

float radToDeg(float rad) {
    return rad * 180 / M_PI;
}

float calculateY(float x) {
    float y = 0, wavelength = 1.0, amplitude = 0.25;
    float k = 2.0 * M_PI / wavelength;
    
    return y = (amplitude * sin(k * x));
}

float calculateYPar(float x) {
    float y = 0;
    return y = 2 * (x * x) + 2 * x + 1;
}

float calculateDerivate(float x){
    return cos((x + global.animationX) * M_PI * 2) * 2 *  M_PI * 0.25;
}

float xFromAngle(float angle, float length){
    return length * cosf(angle);
}

float yFromAngle(float angle, float length){
    return length * sinf(angle);
}

float calculateProjMotion(float dt, float v0y, float ry){
    return 1.0 / 2.0 * global.g * dt * dt + v0y * dt + ry;
}

float calculateDistance(float x1, float x2, float y1, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void drawHealthBar(int max, int curr, float x, float y) {
    float lengthOfPoint = 0.5 / float(max);
    glBegin(GL_QUADS);
    glVertex3f(x, y - 0.05, 0.0);
    glVertex3f(x + float(curr)*lengthOfPoint, y - 0.05, 0.0);
    glVertex3f(x + float(curr)*lengthOfPoint, y , 0.0);
    glVertex3f(x, y, 0.0);
    glEnd();
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
    glColor3f(white.r, white.g, white.b);
}

void drawSquare(float r, float g, float b, float dimensions)
{
    float coord = dimensions / 2.0;
    glBegin(GL_POLYGON);
    glColor3f(r, g, b);
    glVertex3f(-coord, -coord, 0.0);
    glVertex3f(coord, -coord, 0.0);
    glVertex3f(coord, coord, 0.0);
    glVertex3f(-coord, coord, 0.0);
    glEnd();
}

void drawTrapezoid(float r, float g, float b){
    glBegin(GL_POLYGON);
    glColor3f(r, g, b);
    glVertex3f(-0.5, -0.25, 0.0);
    glVertex3f(0.5, -0.25, 0.0);
    glVertex3f(1, 0.25, 0.0);
    glVertex3f(-1.0, 0.25, 0.0);
    glEnd();
}

void drawCircle(float r, float xStart, float yStart) {
    glPushMatrix();
    glBegin(GL_LINE_LOOP);
    glColor3f(white.r, white.g, white.b);
    float x, y;
    for (int i = 0; i <= 36; i++) {
        float deg = i * 10.0;
        x = xStart + cosf(degToRad(deg)) * r;
        y = yStart + sinf(degToRad(deg)) * r;
        glVertex3f(x, y, 0.0);
    }
    glEnd();
    glPopMatrix();
}

void displayProjectile(state& projectile)
{
    float x = 0.01 * cosf(degToRad(60));
    float y = 0.01 * sinf(degToRad(60));
    glPushMatrix();
    
    glTranslatef(projectile.r.x, projectile.r.y, 0);
    if (global.debug)
        printf("r x: %.2f, r y: %.2f\n", projectile.r.x, projectile.r.y);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(pink.r, pink.g, pink.b);
    glVertex3f(0.0, 0.0, 0.01);
    glVertex3f(0.01, 0.0, 0.0);
    glVertex3f(x, y, 0.0);
    glVertex3f(-x, y, 0.0);
    glVertex3f(-0.01, 0.0, 0.0);
    glVertex3f(-x, -y, 0.0);
    glVertex3f(x, -y, 0.0);
    glVertex3f(0.01, 0.0, 0.0);
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0, 0.0, -0.01);
    glVertex3f(0.01, 0.0, 0.0);
    glVertex3f(x, y, 0.0);
    glVertex3f(-x, y, 0.0);
    glVertex3f(-0.01, 0.0, 0.0);
    glVertex3f(-x, -y, 0.0);
    glVertex3f(x, -y, 0.0);
    glVertex3f(0.01, 0.0, 0.0);
    glEnd();
    glColor3f(white.r, white.g, white.b);
    glPopMatrix();
}

void drawIsland(){
    glPushMatrix();
    glColor3f(green.r, green.g, green.b);
    glBegin(GL_QUADS);
    glVertex3f(-ISLAND_BORDERS, -1.0, 0.0);
    glVertex3f(ISLAND_BORDERS, -1.0, 0.0);
    glVertex3f(ISLAND_BORDERS, ISLAND_BORDERS, 0.0);
    glVertex3f(-ISLAND_BORDERS, ISLAND_BORDERS, 0.0);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, 0.25, 0.0);
    glRotatef(island.theta, 0.0, 0.0, 1.0);
    glTranslatef(0.125, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex3f(-0.125, -0.025, 0.0);
    glVertex3f(0.125, -0.025, 0.0);
    glVertex3f(0.125, 0.025, 0.0);
    glVertex3f(-0.125, 0.025, 0.0);
    glEnd();
    glPopMatrix();
}

void drawBoat(float r, float g, float b, float cannon_angle) {
    //    body
    glPushMatrix();
    if (global.axes)
        drawAxes(1.0);
    drawTrapezoid(r, g, b);
    glPopMatrix();
    
    //    turret
    glPushMatrix();
    glTranslatef(0.0, 0.5, 0.0);
    if (global.axes)
        drawAxes(0.2);
    drawSquare(r, g, b, 0.5);
    glPopMatrix();
    
    //    cannon
    glPushMatrix();
    glTranslatef(0.5, 0.25, 0.0);
    if (global.axes)
        drawAxes(1.0);
    glRotatef(cannon_angle, 0.0, 0.0, 1.0);
    if (global.axes)
        drawAxes(0.2);
    glTranslatef(0.25, 0.0, 0.0);
    if (global.axes)
        drawAxes(0.2);
    glScalef(1.0, 0.25, 1.0);
    drawSquare(r, g, b, 0.5);
    glTranslatef(0.25, 0.0, 0.0);
    if (global.axes)
        drawAxes(0.2);
    glPopMatrix();
}

void drawVector(float x, float y, float a, float b, float s, bool normalize, float red, float green, float blue) {
    float z = 0;
    
    glBegin(GL_LINES);
    glColor3f(red, green, blue);
    
    glVertex3f(x, y, z);
    if (normalize) {
        float magnitude = sqrt(a * a + b * b);
        glVertex3f(x + a / magnitude * s, y + b / magnitude * s, z);
    } else {
        glVertex3f(x + a * s, y + b * s, z);
    }
    
    glColor3f(white.r, white.g, white.b);
    glEnd();
}

void drawTrajectoryNumerical(state* projectile_t)
{
    float current_time = glutGet(GLUT_ELAPSED_TIME);
    current_time /= 5000.0;
    
    float left = projectile_t->r.x;
    float dt = delta_t * TIME_APP;
    float multiple_dt = 0.0;
    float start_time = projectile_start_times[projectile_t];
    
    float x = projectile_t->r.x;
    float y = projectile_t->r.y;
    
    glPushMatrix();
    
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 0.862745, 0.294117);
    
    if (global.debug)
        printf("projectile x: %f, projectile y: %f\n", projectile_t->r.x, projectile_t->r.y);
    while (y >= calculateY(x + global.animationX))
    {
        multiple_dt += dt;
        if (multiple_dt >= (current_time * TIME_APP - start_time * TIME_APP))
        {
            x = left + projectile_t->v0.x * multiple_dt;
            y = calculateProjMotion(multiple_dt, projectile_t->v0.y, projectile_t->r.y);

            if (global.debug)
                printf("y: %.2f, at time: %.2f, first part: %.2f\n", y, multiple_dt, 1.0 / 2.0 * global.g * multiple_dt * multiple_dt);
            
            x = x - projectile_t->r.x + projectile_t->r0.x;
            y = y - projectile_t->r.y + projectile_t->r0.y;
            glVertex3f(x, y, 0);
        }
    }
    
    glEnd();
    glPopMatrix();
}

void drawProjState(std::vector<state*> &projectiles) {
    if (projectiles.size() > 0) {
        for(state* projectile_t : projectiles) {
            displayProjectile(*projectile_t);
            drawTrajectoryNumerical(projectile_t);
        }
    }
}

void updateProjectileStateNumerical(state& projectile, float dt)
{
    dt *= TIME_APP;
    
    // Position
    projectile.r.x += projectile.v.x * dt;
    projectile.r.y += projectile.v.y * dt;
    if (global.debug)
        printf("r x: %f, r y: %f\nv x: %f, v y: %f\n", projectile.r.x, projectile.r.y, projectile.v.x,  projectile.v.y);
    
    // Velocity
    projectile.v.y += global.g * dt;
}

void updateProjectileState(state& projectile, float dt)
{
    if ((projectile.r.y == projectile.r0.y) ||
        projectile.r.y + projectile.v.y * dt >= -2.0)
        updateProjectileStateNumerical(projectile, dt);
}

void updateIslandProj() {
    float start_y = 0.25;
    
    island_projectile->r0.x = xFromAngle(degToRad(island.theta), island.r);
    island_projectile->r0.y = start_y + yFromAngle(degToRad(island.theta), island.r);
    
    island_projectile->v0.x = xFromAngle(degToRad(island.theta), 0.25);
    island_projectile->v0.y = start_y + yFromAngle(degToRad(island.theta), 0.25);
}

void convertPolToCar(state& projectile, boat_t& boat, bool blue) {
    float start_x, start_y, angle, c_angle;
    float length = sqrtf(0.025*0.025 + 0.05*0.05);
    
    if (blue) {
        angle = atanf(0.025/0.05);
        c_angle = boat.canon_angle + boat.rotation;
    } else {
        angle = M_PI - atanf(0.025/0.05);
        c_angle = 180 - boat.canon_angle + boat.rotation;
    }
    
    start_x = xFromAngle(angle + degToRad(boat.rotation), length);
    start_y = yFromAngle(angle + degToRad(boat.rotation), length);
    
    projectile.r0.x = start_x + xFromAngle(degToRad(c_angle), boat.cannon_length * 0.1);
    projectile.r0.y = start_y + yFromAngle(degToRad(c_angle), boat.cannon_length * 0.1);
    
    projectile.v0.x = (start_x + xFromAngle(degToRad(c_angle), 0.5));
    projectile.v0.y = (start_y + yFromAngle(degToRad(c_angle), 0.5));
}

void addProjectile(std::vector<state*>& projs, state& projectile, float xPos, float yPos)
{
    state* projectile_n = new state();
    projectile_n->r0.x =  projectile.r0.x + xPos;
    projectile_n->r0.y =  projectile.r0.y + yPos;
    projectile_n->v0.x =  projectile.v0.x;
    projectile_n->v0.y =  projectile.v0.y;
    projectile_n->r.x  =  projectile.r0.x + xPos;
    projectile_n->r.y  =  projectile.r0.y + yPos;
    projectile_n->v.x =  projectile.v0.x;
    projectile_n->v.y =  projectile.v0.y;
    projs.push_back(projectile_n);
    float start_time = glutGet(GLUT_ELAPSED_TIME);
    start_time /= 5000.0;
    projectile_start_times.insert(std::pair<state*, float>(projectile_n, start_time));
}

void detectCollision(std::vector<state*> &projectiles, int boat_col, bool isIsland, bool defence){
    float boat_r = 0.1 * sqrt(1 + 0.25 * 0.25);
    bool erase = false;
    int i = 0;
    for (state* projectile : projectiles) {
        erase = false;
        float d = calculateDistance(projectile->r.x, boats[boat_col]->position.x, projectile->r.y, boats[boat_col]->position.y);
        if (projectile->r.y < calculateY(projectile->r.x + global.animationX))
            erase = true;
        if (d < boat_r + 0.01 && !defence) {
            boats[boat_col]->health--;
            if (global.debug)
                printf("%d boat health: %d\n", boat_col, boats[boat_col]->health);
            erase = true;
        }
        if (!isIsland) {
            if (projectile->r.x >= -ISLAND_BORDERS && projectile->r.x <= ISLAND_BORDERS && projectile->r.y <= ISLAND_BORDERS) {
                island.health--;
                erase = true;
                if (global.debug)
                    printf("Island health: %d\n", island.health);
            }
        }
        if (projectile->r.x > 1.0 || projectile->r.x < -1.0)
            erase = true;
        if (defence) {
            float r = 0.0;
            if (boat_col == BLUE)
                d = calculateDistance(blue_defence_projectile->r.x, projectile->r.x, blue_defence_projectile->r.y, projectile->r.y);
                r = blue_defence.r;
            if (boat_col == RED)
                d = calculateDistance(red_defence_projectile->r.x, projectile->r.x, red_defence_projectile->r.y, projectile->r.y);
                r = red_defence.r;
            if (d < 0.01 + r)
                erase = true;
        }
        if (erase){
            projectile_start_times.erase(projectile);
            projectiles.erase(projectiles.begin() + i);
            i--;
        }
        i++;
    }
}

void updateProjectiles(std::vector<state*> &projectiles, float dt){
    if (projectiles.size() > 0){
        for(state* projectile_t : projectiles) {
            updateProjectileState(*projectile_t, dt);
        }
    }
}

void defenceUpdate(defence_t& defence, state* projectile) {
    if (defence.active) {
        if(glutGet(GLUT_ELAPSED_TIME) - defence.start_time < 300.0)
            defence.r += 0.01;
        else{
            projectile->r0.x = 0.0;
            projectile->r0.y = 0.0;
            projectile->v0.x = 0.0;
            projectile->v0.y = 0.0;
            projectile->r.x = 0.0;
            projectile->r.y = 0.0;
            projectile->v.x = 0.0;
            projectile->v.y = 0.0;
            defence.active=false;
        }
    }
}

void drawWater()
{
    float left = -1.0;
    float right = 1.0;
    float range = right - left;
    float stepSize = range / global.n;
    
    float x = 0.0;
    float y = 0;
    
    glColor4f(0.4, 1.0, 1.0, 0.75);
    
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= global.n; i++) {
        x = (i * stepSize + left);
        y = (calculateY(x + global.animationX));
        glVertex3f(x, y, 0);
        glVertex3f(x, -1.0, 0.0);
    }
    glEnd();
    
    if (global.tangents || global.normals) {
        for(int i = 0; i <= global.n; i++) {
            x = i * stepSize + left;
            y = calculateY(x + global.animationX);
            if (global.tangents)
                drawVector(x, y, 1.0, calculateDerivate(x), 0.1, true, 1, 0, 0);
            if(global.normals)
                drawVector(x, y, -calculateDerivate(x), 1.0, 0.1, true, 0, 1, 0);
        }
    }
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
    glColor3f(white.r, white.g, white.b);
    glRasterPos2i(270, 370);
    snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", global.frame_rate);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    
    /* Time per frame */
    glColor3f(white.r, white.g, white.b);
    glRasterPos2i(270, 350);
    snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / global.frame_rate * 1000.0);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    
    if (!global.playing) {
        glColor3f(1.0, 0.0, 0.0);
        glRasterPos2i(w /2 - 35, h / 2);
        snprintf(buffer, sizeof buffer, "GAME OVER");
        for (bufp = buffer; *bufp; bufp++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *bufp);
    }
    
    /* Pop modelview */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    
    /* Pop projection */
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* Pop attributes */
    glPopAttrib();
}

void initDefence(state* projectile, float xPos, float yPos) {
    projectile->r0.x =  projectile->r0.x + xPos;
    projectile->r0.y =  projectile->r0.y + yPos;
    projectile->r.x  =  projectile->r0.x;
    projectile->r.y  =  projectile->r0.y;
    projectile->v.x  =  projectile->v0.x;
    projectile->v.y  =  projectile->v0.y;
}

void updateDefenceState(state* projectile, int boat_i, bool blue, float r) {
    convertPolToCar(*projectile, *boats[boat_i], blue);
    if (projectile->v.x == 0.0)
    {
        initDefence(projectile, boats[boat_i]->position.x, boats[boat_i]->position.y);
    }
    drawCircle(r, projectile->r.x, projectile->r.y);
}

void display(void)
{
    GLenum err;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (global.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if (global.axes)
        drawAxes(1.0);
    
    glPushMatrix();
    drawIsland();
    glPopMatrix();
    
    //    blue boat starting at the lefthand side
    glPushMatrix();
    glTranslatef(boats[BLUE]->position.x, boats[BLUE]->position.y, 0.0);
    glScalef(0.1, 0.1, 0.0);
    glRotatef(boats[BLUE]->rotation, 0.0, 0.0, 1.0);
    drawBoat(blue.r, blue.g, blue.b, boats[BLUE]->canon_angle);
    glPopMatrix();
    
    //    red boat starting at the righthand side
    glPushMatrix();
    glTranslatef(boats[RED]->position.x, boats[RED]->position.y, 0.0);
    glScalef(0.1, 0.1, 0.0);
    glRotatef(boats[RED]->rotation, 0.0, 0.0, 1.0);
    glRotatef(180, 0.0, 1.0, 0.0);
    drawBoat(red.r, red.g, red.b, boats[RED]->canon_angle);
    glPopMatrix();
    
    drawWater();
    
    if (blue_defence.active) {
        updateDefenceState(blue_defence_projectile, BLUE, true, blue_defence.r);
    }
    
    if (red_defence.active) {
        updateDefenceState(red_defence_projectile, RED, false, red_defence.r);
    }
    
    drawProjState(blue_projectiles);
    drawProjState(red_projectiles);
    drawProjState(island_projectiles);
    
    convertPolToCar(boats[BLUE]->projectile, *boats[BLUE], true);
    convertPolToCar(boats[RED]->projectile, *boats[RED], false);
    updateIslandProj();
    
    glColor3f(blue.r, blue.g, blue.b);
    drawHealthBar(10, boats[BLUE]->health, -0.95, 0.95);
    glColor3f(red.r, red.g, red.b);
    drawHealthBar(10, boats[RED]->health, -0.95, 0.9);
    glColor3f(green.r, green.g, green.b);
    drawHealthBar(100, island.health, -0.95, 0.85);
    glColor3f(white.r, white.g, white.b);
    
    glPushMatrix();
    displayOSD();
    glPopMatrix();
    
    glutSwapBuffers();
    
    global.frames++;
    
    // Check for errors
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("%s\n",gluErrorString(err));
}

void keyboardCB(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
            boats.clear();
            blue_projectiles.clear();
            red_projectiles.clear();
            island_projectiles.clear();
            projectile_start_times.clear();
            delete island_projectile;
            exit(EXIT_SUCCESS);
            break;
        case 'w':
            global.wireframe = !global.wireframe;
            break;
        case 't':
            global.tangents = !global.tangents;
            break;
        case 'x':
            global.axes = !global.axes;
            break;
        case 'n':
            global.normals = !global.normals;
            break;
        case '+':
            global.n *= 2;
            break;
        case '-':
            if (global.n > 4)
                global.n /= 2;
            break;
        case 'a':
            if (global.playing)
                boats[BLUE]->movement = LEFT;
            break;
        case 'd':
            if (global.playing)
                boats[BLUE]->movement = RIGHT;
            break;
        case 'j':
            if (global.playing)
                boats[RED]->movement = LEFT;
            break;
        case 'l':
            if (global.playing)
                boats[RED]->movement = RIGHT;
            break;
        case 's':
            if (global.playing){
                if (blue_defence.start_time == 0.0 || glutGet(GLUT_ELAPSED_TIME) - blue_defence.start_time > 1000.0) {
                    blue_defence.r = 0.05;
                    blue_defence.start_time = glutGet(GLUT_ELAPSED_TIME);
                    blue_defence.active = true;
                }
            }
            break;
        case 'k':
            if (global.playing) {
                if (red_defence.start_time == 0.0 || glutGet(GLUT_ELAPSED_TIME) - red_defence.start_time > 1000.0) {
                    red_defence.r = 0.05;
                    red_defence.start_time = glutGet(GLUT_ELAPSED_TIME);
                    red_defence.active = true;
                }
            }
            break;
        case 'g':
            if (global.playing)
                global.animation = !global.animation;
            break;
        case 'h':
            if (global.playing){
                if (island.theta - ANGLE_STEP >= 0)
                    island.angle_d = DECREASE;
            }
            break;
        case 'f':
            if (global.playing){
                if (island.theta + ANGLE_STEP >= 0)
                    island.angle_d = INCREASE;
            }
            break;
        case 'q':
            if (global.playing){
                if (boats[BLUE]->canon_angle - ANGLE_STEP >= 0)
                    boats[BLUE]->angle_d = DECREASE;
            }
            break;
        case 'Q':
            if (global.playing){
                if (boats[BLUE]->canon_angle + ANGLE_STEP <= 180)
                    boats[BLUE]->angle_d = INCREASE;
            }
            break;
        case 'o':
            if (global.playing){
                if (boats[RED]->canon_angle - ANGLE_STEP >= 0)
                    boats[RED]->angle_d = DECREASE;
            }
            break;
        case 'O':
            if (global.playing){
                if (boats[RED]->canon_angle + ANGLE_STEP <= 180)
                    boats[RED]->angle_d = INCREASE;;
            }
            break;
        case 'e':
            if (global.playing){
                if (blue_time_f == 0.0 || glutGet(GLUT_ELAPSED_TIME) - blue_time_f > 1000.0) {
                    blue_time_f = glutGet(GLUT_ELAPSED_TIME);
                    new_projectile.blue = true;
                }
            }
            break;
        case 'i':
            if (global.playing){
                if (red_time_f == 0.0 || glutGet(GLUT_ELAPSED_TIME) - red_time_f > 1000.0) {
                    red_time_f = glutGet(GLUT_ELAPSED_TIME);
                    new_projectile.red = true;
                }}
            break;
        case ' ':
            if (global.playing){
                if (island_time_f == 0.0 || glutGet(GLUT_ELAPSED_TIME) - island_time_f > 1000.0) {
                    island_time_f = glutGet(GLUT_ELAPSED_TIME);
                    new_projectile.island = true;
                }}
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch (key) {
        case 'a':
        case 'd':
            boats[BLUE]->movement = STILL;
            break;
        case 'j':
        case 'l':
            boats[RED]->movement = STILL;
            break;
        case 'q':
        case 'Q':
            boats[BLUE]->angle_d = NO_MOVE;
            break;
        case 'o':
        case 'O':
            boats[RED]->angle_d = NO_MOVE;
            break;
        case 'h':
        case 'f':
            island.angle_d = NO_MOVE;
            break;
        default:
            break;
    }
    
    glutPostRedisplay();
}

void myReshape(int w, int h)
{
    //    TODO: remove *2
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
    float t, dt;
    /* Get elapsed time and convert to s */
    t = glutGet(GLUT_ELAPSED_TIME);
    t /= 5000.0;
    
    //    Chnage in time
    dt = t - t_last;
    
    if (boats[RED]->health == 0 || boats[BLUE]->health == 0 || island.health == 0) {
        global.animation = false;
        global.playing = false;
    }
    
    defenceUpdate(blue_defence, blue_defence_projectile);
    defenceUpdate(red_defence, red_defence_projectile);
    
    detectCollision(red_projectiles, BLUE, false, blue_defence.active);
    detectCollision(blue_projectiles, RED, false, red_defence.active);
    detectCollision(island_projectiles, RED, true, red_defence.active);
    detectCollision(island_projectiles, BLUE, true, blue_defence.active);
    
    /* Update velocity and position */
    if (global.animation)
        global.animationX += dt;
    
    if (new_projectile.blue)
    {
        addProjectile(blue_projectiles, boats[BLUE]->projectile, boats[BLUE]->position.x, boats[BLUE]->position.y);
        new_projectile.blue = false;
    }
    
    if (new_projectile.red)
    {
        addProjectile(red_projectiles, boats[RED]->projectile, boats[RED]->position.x, boats[RED]->position.y);
        new_projectile.red = false;
    }
    
    if (new_projectile.island)
    {
        addProjectile(island_projectiles, *island_projectile, 0.0, 0.0);
        new_projectile.island = false;
    }
    
    //    movement dependent on delta t for BLUE boat
    boats[BLUE]->position.y = (calculateY(boats[BLUE]->position.x + global.animationX));
    
    //    movement dependent on delta t BLUE
    if (boats[BLUE]->movement == LEFT && boats[BLUE]->position.x > -1.0){
        boats[BLUE]->position.x -= dt * 1.5;
    } else if (boats[BLUE]->movement == RIGHT && boats[BLUE]->position.x < 1.0) {
        boats[BLUE]->position.x += dt * 1.5;
    }
    
    if (boats[BLUE]->angle_d == INCREASE){
        boats[BLUE]->canon_angle += ANGLE_STEP;
    } else if (boats[BLUE]->angle_d == DECREASE) {
        boats[BLUE]->canon_angle -= ANGLE_STEP;
    }
    
    boats[BLUE]->rotation = radToDeg(atan((calculateDerivate(boats[BLUE]->position.x))/1.0));
    
    //    movement RED boat
    boats[RED]->position.y = (calculateY(boats[RED]->position.x + global.animationX));
    
    //    movement dependent on delta t RED
    if (boats[RED]->movement == LEFT && boats[RED]->position.x > -1.0)
        boats[RED]->position.x -= dt * 1.5;
    else if (boats[RED]->movement == RIGHT && boats[RED]->position.x < 1.0)
        boats[RED]->position.x += dt * 1.5;
    
    if (boats[RED]->angle_d == INCREASE){
        boats[RED]->canon_angle += ANGLE_STEP;
    } else if (boats[RED]->angle_d == DECREASE) {
        boats[RED]->canon_angle -= ANGLE_STEP;
    }
    
    boats[RED]->rotation = radToDeg(atan((calculateDerivate(boats[RED]->position.x))/1.0));
    
    if (island.angle_d == INCREASE){
        island.theta += ANGLE_STEP;
    } else if (island.angle_d == DECREASE) {
        island.theta -= ANGLE_STEP;
    }
    
    //    update last time
    t_last = t;
    delta_t = dt;
    
    updateProjectiles(blue_projectiles, dt);
    updateProjectiles(red_projectiles, dt);
    updateProjectiles(island_projectiles, dt);
    
    if (blue_defence.active)
        updateProjectileStateNumerical(*blue_defence_projectile, dt);
    if (red_defence.active)
        updateProjectileStateNumerical(*red_defence_projectile, dt);
    
    global.dt += dt;
    dt = t - global.last_frame_rate_t;
    if (dt > global.frame_rate_interval) {
        global.frame_rate = global.frames / global.dt / 5;
        global.last_frame_rate_t = t;
        global.frames = 0;
        global.dt = 0;
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
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Boat Game");
    init();
    
    glutKeyboardFunc(keyboardCB);
    glutKeyboardUpFunc(keyboardUp);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutIdleFunc(update);
    
    glutMainLoop();
}

