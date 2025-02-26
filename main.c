//
//  main.c
//  3DProjection
//
//  Created by Illuminum on 2/24/25.
//  Semi-port of my Pygame 3D projection project to C
//

#include "blackbox.h"

#define BLACKBOX_TIMEOUT_1 1
#define BLACKBOX_TIMEOUT_2 125

BlackBox* blackbox;

// These functions are called when the buttons are pressed
void on_up() {}
void on_down() {}
void on_left() {}
void on_right() {}
void on_select() {}

// These functions are called repeatedly
void on_timeout_1() {}
void on_timeout_2() {}

const int FOCAL_LENGTH = 10;
const float SPEED = 0.1;

const float PI = 3.1415926535;
const int TERMS = 7;
const int SQROOT_ITER = 32; // Could probably be lower

float x = 0.0;
float y = 0.0;
float z = -2.0;
float quat0 = 1.0;
float quat1 = 0.0;
float quat2 = 0.0;
float quat3 = 0.0;
float focalLength = FOCAL_LENGTH;
float speed = SPEED;

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

float sine(float deg) { // Modified from https://stackoverflow.com/questions/38917692/sin-cos-funcs-without-math-h
    //deg %= 360; // make it less than 360
    deg = min(deg, 360);
    float rad = deg * PI / 180;
    float sin = 0;

    for(int i = 0; i < TERMS; i++) { // That's Taylor series!!
        sin += power(-1, i) * power(rad, 2 * i + 1) / fact(2 * i + 1);
    }
    return sin;
}

float cosine(float deg) {
    //deg %= 360; // make it less than 360
    deg = min(deg, 360);
    float rad = deg * PI / 180;
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

int toInt(float num) {
    if (num < -0.5) {
        return -1;
    }
    if (num < 0.5) {
        return 0;
    }
    if (num < 1.5) {
        return 1;
    }
    if (num < 2.5) {
        return 2;
    }
    if (num < 3.5) {
        return 3;
    }
    if (num < 4.5) {
        return 4;
    }
    if (num < 5.5) {
        return 5;
    }
    if (num < 6.5) {
        return 6;
    }
    if (num < 7.5) {
        return 7;
    }
    return -1;
}

// PROJECTION //

int project(float p0, float p1, float p2) {
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

// MATRICES AND QUATERNIONS //

void multiplyRWithQuat(float q20, float q21, float q22, float q23) { // Stores result in first quaternion
    float w1 = quat0; // For better readability
    float x1 = quat1;
    float y1 = quat2;
    float z1 = quat3;

    float w2 = q20;
    float x2 = q21;
    float y2 = q22;
    float z2 = q23;

    quat0 = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;
    quat1 = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
    quat2 = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
    quat3 = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;
}

void normalizeR() {
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
    x += (R00 * speed * m0) + (R01 * speed * m1) + (R02 * speed * m2);
    y += (R10 * speed * m0) + (R11 * speed * m1) + (R12 * speed * m2);
    z += (R20 * speed * m0) + (R21 * speed * m1) + (R22 * speed * m2);
}

void rotateCamera(float deltaAngleDegrees, float a0, float a1, float a2) {
    float deltaAngle = (deltaAngleDegrees * (PI/180)) / 2;
    float deltaQ0 = cosine(deltaAngle);
    float deltaQ1 = a0 * sine(deltaAngle);
    float deltaQ2 = a1 * sine(deltaAngle);
    float deltaQ3 = a2 * sine(deltaAngle);

    multiplyRWithQuat(deltaQ0, deltaQ1, deltaQ2, deltaQ3);
    normalizeR();
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

void main() {
    while (1) {
        // Point to project
        //float point[3] = {0, 2, 2};
        float p0 = 0;
        float p1 = 0;
        float p2 = 2;

        // Project point
        //project(R, R_inv, point, &xPos, &yPos);
        project(p0, p1, p2);

        //updateR(); // Update rotation matrix after movement

        blackbox.matrix.pixel(toInt(xProjected)+4, toInt(yProjected)+4).turn_on();
    }
}