//
//  main.c
//  3DProjection
//
//  Created by Illuminum on 2/24/25 for the Blackbox YSWS
//  Repo: https://github.com/Illuminum2/c-3d-projection
//  Run: https://blackbox.hackclub.com/editor/?code=2e5f68
//  Semi-port of my Pygame 3D projection project to C, heavily modified
//

//
//  Controls
//  Up - 1: Move forward 2: Move Up
//  Down - 1: Move backward 2: Move Down
//  Left - 1: Move left 2:Look left
//  Right - 1: Move right 2: Look right
//  X - Switch between 1 and 2
//

#include "blackbox.h"

#define BLACKBOX_TIMEOUT_1 1
#define BLACKBOX_TIMEOUT_2 125

BlackBox* blackbox;

const int FOCAL_LENGTH = 289;
const int SIMULATED_DISPLAY_SIZE = 1000;
const int REAL_DISPLAY_SIZE = 8;
const int DISPLAY_MIN = -SIMULATED_DISPLAY_SIZE/2;

const float SPEED = 0.1*20;
const float ROT_SPEED_SIN = 0.2588190451; // sin of 0.087 rad(~5 degrees)
const float ROT_SPEED_COS = 0.9659258263; // cos of 0.087 rad(~5 degrees)

const float PI = 3.1415926535;
const int TERMS = 7;
const int SQROOT_ITER = 32; // Could probably be lower

float x = 0.0;
float y = 0.0;
float z = 0.0;
float quat0 = 1.0;
float quat1 = 0.0;
float quat2 = 0.0;
float quat3 = 0.0;

// Identity matrix
//float R[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}};
float R00 = 1;
float R01 = 0;
float R02 = 0;
float R10 = 0;
float R11 = 1;
float R12 = 0;
float R20 = 0;
float R21 = 0;
float R22 = 1;

float xProjected; // float xProjected, yProjected; does not work
float yProjected;

// MOVEMENT //

float moveX = 0;
float moveY = 0;
float moveZ = 0;

float turnX = 0;
float turnY = 0;
float turnZ = 0;

int MTMode = 0; // Movement is 0, Turning is 1
int DPUpdate = 1; // Has the position/rotation of the camera been changed

// Functions called when buttons are pressed
void on_up() {
    if (MTMode == 0) {
        moveZ += 1;
    }
    else {
        moveY -= 1;
    }
    DPUpdate = 1;
}
void on_down() {
    if (MTMode == 0) {
        moveZ -= 1;
    }
    else {
        moveY += 1;
    }
    DPUpdate = 1;
}
void on_left() {
    if (MTMode == 0) {
        moveX -= 1;
    }
    else {
        // Temporary rotation fix, simpler direct matrix rotation
        R00 = R00 * ROT_SPEED_COS + R02 * ROT_SPEED_SIN;
        R02 = -R00 * ROT_SPEED_SIN + R02 * ROT_SPEED_COS;

        R10 = R10 * ROT_SPEED_COS + R12 * ROT_SPEED_SIN;
        R12 = -R10 * ROT_SPEED_SIN + R12 * ROT_SPEED_COS;

        R20 = R20 * ROT_SPEED_COS + R22 * ROT_SPEED_SIN;
        R22 = -R20 * ROT_SPEED_SIN + R22 * ROT_SPEED_COS;

        //blackbox.matrix.turn_all_off();
    }
    DPUpdate = 1;
}
void on_right() {
    if (MTMode == 0) {
        moveX += 1;
    }
    else {
        R00 = R00 * ROT_SPEED_COS - R02 * ROT_SPEED_SIN;
        R02 = R00 * ROT_SPEED_SIN + R02 * ROT_SPEED_COS;

        R10 = R10 * ROT_SPEED_COS - R12 * ROT_SPEED_SIN;
        R12 = R10 * ROT_SPEED_SIN + R12 * ROT_SPEED_COS;

        R20 = R20 * ROT_SPEED_COS - R22 * ROT_SPEED_SIN;
        R22 = R20 * ROT_SPEED_SIN + R22 * ROT_SPEED_COS;

        //blackbox.matrix.turn_all_off();
    }
    DPUpdate = 1;
}
void on_select() {
    if (MTMode == 0) {
        MTMode = 1;
    }
    else {
        MTMode = 0;
    }
    DPUpdate = 1;
}

// These functions are called repeatedly
void on_timeout_1() {}
void on_timeout_2() {}

// MATH //

float power(float base, int exp) {
    if(exp < 0) {
        if(base == 0) { // if without {} does not work
            return -0; // Error
        }
        return 1 / (base * power(base, (-exp) - 1));
    }
    if(exp == 0) {
        return 1;
    }
    if(exp == 1) {
        return base;
    }
    return base * power(base, exp - 1);
}

int fact(int n) {
    return n <= 0 ? 1 : n * fact(n-1);
}

float min(float i, float j) {
    if (i < j) {
        return i;
    }
    return j;
}

float max(float i, float j) {
    if (i > j) {
        return i;
    }
    return j;
}

float sine(float rad) { // Modified from https://stackoverflow.com/questions/38917692/sin-cos-funcs-without-math-h
    float sin = 0;

    for(int i = 0; i < TERMS; i++) { // That's Taylor series!!
        sin += power(-1, i) * power(rad, 2 * i + 1) / fact(2 * i + 1);
    }
    return sin;
}

float cosine(float rad) {
    float cos = 0;

    for(int i = 0; i < TERMS; i++) { // That's also Taylor series!!
        cos += power(-1, i) * power(rad, 2 * i) / fact(2 * i);
    }
    return cos;
}

float sqroot(float square) // Modified from https://stackoverflow.com/questions/29018864/any-way-to-obtain-square-root-of-a-number-without-using-math-h-and-sqrt
{
    float root=square/3;
    if (square <= 0) { // One-line if does not work
      return 0;
    }
    for (int i = 0; i < SQROOT_ITER; i++) { // for without {} does not work
        root = (root + square / root) / 2;
    }
    return root;
}

// MATRICES AND QUATERNIONS //

void multiplyQWithQuat(float q20, float q21, float q22, float q23) { // Multiply camera quaternion with quaternion
    float w1 = quat0; // For better readability
    float x1 = quat1;
    float y1 = quat2;
    float z1 = quat3;

    float w2 = q20;
    float x2 = q21;
    float y2 = q22;
    float z2 = q23;

    quat0 = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2; // Result stored in camera quaternion
    quat1 = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
    quat2 = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
    quat3 = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;
}

void normalizeQ() { // Normalize camera quaternion
    float w1 = quat0; // For better readability
    float x1 = quat1;
    float y1 = quat2;
    float z1 = quat3;

    float length = sqroot(w1 * w1 + x1 * x1 + y1 * y1 + z1 * z1);

    quat0 = w1 / length;
    quat1 = x1 / length;
    quat2 = y1 / length;
    quat3 = z1 / length;
}

// CAMERA //

void setCamera(float xNew, float yNew, float zNew) {
    x = xNew;
    y = yNew;
    z = zNew;
}

void moveCamera(float m0, float m1, float m2) {
    x += (R00 * SPEED * m0) + (R01 * SPEED * m1) + (R02 * SPEED * m2);
    y += (R10 * SPEED * m0) + (R11 * SPEED * m1) + (R12 * SPEED * m2);
    z += (R20 * SPEED * m0) + (R21 * SPEED * m1) + (R22 * SPEED * m2);
}

void rotateCamera(float deltaAngleDegrees, float a0, float a1, float a2) {
    float deltaAngle = (min(deltaAngleDegrees, 360) * (PI / 180)) / 2;
    float deltaQ0 = cosine(deltaAngle);
    float deltaQ1 = a0 * sine(deltaAngle);
    float deltaQ2 = a1 * sine(deltaAngle);
    float deltaQ3 = a2 * sine(deltaAngle);

    multiplyQWithQuat(deltaQ0, deltaQ1, deltaQ2, deltaQ3);
    normalizeQ();
}

void updateR() {
    float w1 = quat0;
    float x1 = quat1;
    float y1 = quat2;
    float z1 = quat3;

    R00 = 1 - 2 * y1 * y1 - 2 * z1 * z1;
    R01 = 2 * x1 * y1 - 2 * z1 * w1;
    R02 = 2 * x1 * z1 + 2 * y1 * w1;

    R10 = 2 * x1 * y1 + 2 * z1 * w1;
    R11 = 1 - 2 * x1 * x1 - 2 * z1 * z1;
    R12 = 2 * y1 * z1 - 2 * x1 * w1;

    R20 = 2 * x1 * z1 - 2 * y1 * w1;
    R21 = 2 * y1 * z1 + 2 * x1 * w1;
    R22 = 1 - 2 * x1 * x1 - 2 * y1 * y1;
}

// MOVEMENT/TURN UPDATE //

void mtUpdate() {
    if (moveX != 0 || moveY != 0 || moveZ != 0) {
        moveCamera(moveX, moveY, moveZ);

        moveX = 0;
        moveY = 0;
        moveZ = 0;

        //blackbox.matrix.turn_all_off();
    }

    if (turnX != 0 || turnY != 0 || turnZ != 0) {
        if (turnX != 0) {
            rotateCamera(turnX, 1, 0, 0);
            updateR();
            turnX = 0;
        }
        if (turnY != 0) {
            rotateCamera(turnY, 0, 1, 0);
            updateR();
            turnY = 0;
        }
        if (turnZ != 0) {
            rotateCamera(turnZ, 0, 0, 1);
            updateR();
            turnZ = 0;
        }

        //blackbox.matrix.turn_all_off();
    }
}

// PROJECTION //

int project(float p0, float p1, float p2) { // Project point
    //project(R, R_inv, point, &xPos, &yPos);

    float P1x = p0 - x;
    float P1y = p1 - y;
    float P1z = p2 - z;

    float P2x = (R00 * P1x) + (R10 * P1y) + (R20 * P1z); // Hardcoded R_inv
    float P2y = (R01 * P1x) + (R11 * P1y) + (R21 * P1z);
    float P2z = (R02 * P1x) + (R12 * P1y) + (R22 * P1z);

    if (P2z > 0) {
        xProjected = FOCAL_LENGTH * P2x / P2z;
        yProjected = FOCAL_LENGTH * P2y / P2z;
        return 1;
    }
    return 0;
}

int scale(float num) {
    float base = DISPLAY_MIN;
    float increment = SIMULATED_DISPLAY_SIZE/REAL_DISPLAY_SIZE;
    if (num < base) {
        return -1;
    }
    if (num < base+increment*1) {
        return 0;
    }
    if (num < base+increment*2) {
        return 1;
    }
    if (num < base+increment*3) {
        return 2;
    }
    if (num < base+increment*4) {
        return 3;
    }
    if (num < base+increment*5) {
        return 4;
    }
    if (num < base+increment*6) {
        return 5;
    }
    if (num < base+increment*7) {
        return 6;
    }
    if (num < base+increment*8) {
        return 7;
    }
    return -1;
}

// DRAW //

void draw(float xProj, float yProj) {
    int xDraw = scale(xProj);
    int yDraw = scale(yProj);

    if (xDraw != -1 && yDraw != -1) {
        blackbox.matrix.pixel_xy(xDraw, yDraw).turn_on();
    }
}

// MAIN //

void main() {
    // Point to project
    //float point[3] = {0, 2, 2};
    float p00 = -3.0; // Point 1
    float p01 = 2.0;
    float p02 = 2.0;

    float p0x;
    float p0y;

    float p10 = 3.0; // Point 2
    float p11 = 2.0;
    float p12 = 2.0;

    float p1x;
    float p1y;

    float p20 = 0.0; // Point 3
    float p21 = -4.0;
    float p22 = 2.0;

    float p2x;
    float p2y;
    setCamera(0, 0, -2); // Maybe remove

    while (1) {
        if (DPUpdate == 1) {
				DPUpdate = 0;
				mtUpdate();

				if (project(p00, p01, p02)) { // Draw point 1
            p0x = xProjected;
            p0y = yProjected;
        }
        else {
            p0x = -1;
            p0y = -1;
        }
        if (project(p10, p11, p12)) { // Draw point 2
            p1x = xProjected;
            p1y = yProjected;
        }
        else {
            p1x = -1;
            p1y = -1;
        }
        if (project(p20, p21, p22)) { // Draw point 3
            p2x = xProjected;
            p2y = yProjected;
        }
        else {
            p2x = -1;
            p2y = -1;
        }

    		blackbox.matrix.turn_all_off();

      	draw(p0x, p0y);
        draw(p1x, p1y);
        draw(p2x, p2y);
      }

      	//blackbox.sleep(100);
    }
}