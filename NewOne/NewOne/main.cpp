// =============================================================
// MERGED OPENGL COMPUTER GRAPHICS PROJECT
// Scene 1 + Scene 2 in one main.cpp
// -------------------------------------------------------------
// Controls:
//   1 = Scene 1
//   2 = Scene 2
//   N = Night
//   D = Day
//   R = Rain
//   P / SPACE = Pause
// =============================================================

#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

// =============================================================
// GLOBAL VARIABLES & ANIMATION STATES
// =============================================================
int currentScene = 1;

float car1X = -1.30f, car2X = -0.65f, car3X = -0.95f;
float bus1X = 0.15f, bus2X = -0.35f;

float planeX = -1.40f;
float planeParam = 0.0f;
float wheelAngle = 0.0f;

float cloud1X = -0.70f, cloud2X = 0.20f, cloud3X = -1.20f;

bool isNight = false, isRaining = false, isPaused = false;

const int MAX_RAIN = 120;
float rainX[MAX_RAIN], rainY[MAX_RAIN], rainSpeed[MAX_RAIN];

// ==========================================
// CHAPTER 3: HOMOGENEOUS 2D TRANSFORMATION STRUCT
// ==========================================
struct Point2D {
    float x, y;
};

Point2D applyTransform(float x, float y, float tx, float ty, float angleDeg, float sx, float sy) {
    float rad = angleDeg * 3.14159265f / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);

    float scaledX = x * sx;
    float scaledY = y * sy;

    float rotX = scaledX * cosA - scaledY * sinA;
    float rotY = scaledX * sinA + scaledY * cosA;

    Point2D p;
    p.x = rotX + tx;
    p.y = rotY + ty;
    return p;
}

// ==========================================
// CHAPTER 5: CUBIC BÉZIER CURVE FOR AIRPLANE PATH
// ==========================================
Point2D calculateBezierPoint(float t, Point2D p0, Point2D p1, Point2D p2, Point2D p3) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Point2D p;
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    return p;
}

// ==========================================
// INITIALIZATION FUNCTIONS
// ==========================================
void initRain() {
    for (int i = 0; i < MAX_RAIN; i++) {
        rainX[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        rainY[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        rainSpeed[i] = 0.025f + ((float)rand() / RAND_MAX) * 0.02f;
    }
}

// ==========================================
// BASIC PRIMITIVES & UTILITY DRAWING
// ==========================================
void drawRect(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 15) {
        float rad = i * 3.14159265f / 180.0f;
        glVertex2f(cx + cos(rad) * radius, cy + sin(rad) * radius);
    }
    glEnd();
}

void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float width) {
    glColor3f(r, g, b);
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawText(float x, float y, float r, float g, float b, const char* text) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// ==========================================
// REAL-WORLD ADDITIONS: CRANE & BLUE FENCE
// ==========================================
void drawConstructionCrane(float x, float y) {
    drawRect(x - 0.015f, y, x + 0.015f, y + 0.75f, 0.85f, 0.50f, 0.10f);

    for (float cy = y; cy < y + 0.70f; cy += 0.05f) {
        drawLine(x - 0.015f, cy, x + 0.015f, cy + 0.05f, 0.30f, 0.20f, 0.05f, 1.5f);
        drawLine(x + 0.015f, cy, x - 0.015f, cy + 0.05f, 0.30f, 0.20f, 0.05f, 1.5f);
    }

    drawLine(x, y + 0.72f, x - 0.25f, y + 0.68f, 0.85f, 0.50f, 0.10f, 3.0f);
    drawLine(x, y + 0.72f, x + 0.10f, y + 0.70f, 0.85f, 0.50f, 0.10f, 3.0f);
    drawRect(x - 0.03f, y + 0.68f, x + 0.02f, y + 0.73f, 0.20f, 0.20f, 0.22f);
    drawLine(x - 0.25f, y + 0.68f, x - 0.25f, y + 0.45f, 0.20f, 0.20f, 0.20f, 1.0f);
}

void drawBlueFence() {
    drawRect(-1.0f, -0.48f, 1.0f, -0.36f, 0.12f, 0.32f, 0.58f);
    for (float x = -1.0f; x <= 1.0f; x += 0.03f) {
        drawLine(x, -0.48f, x, -0.36f, 0.08f, 0.22f, 0.42f, 1.0f);
    }
}

// ==========================================
// SCENERY & ENVIRONMENT RENDERING
// ==========================================
void drawCloud(float x, float y, float scale) {
    float bR = isRaining ? (isNight ? 0.10f : 0.40f) : (isNight ? 0.20f : 0.90f);
    float bG = isRaining ? (isNight ? 0.12f : 0.43f) : (isNight ? 0.24f : 0.92f);
    float bB = isRaining ? (isNight ? 0.16f : 0.48f) : (isNight ? 0.32f : 0.96f);
    float hR = isRaining ? (isNight ? 0.15f : 0.50f) : (isNight ? 0.28f : 1.00f);
    float hG = isRaining ? (isNight ? 0.17f : 0.53f) : (isNight ? 0.32f : 1.00f);
    float hB = isRaining ? (isNight ? 0.22f : 0.58f) : (isNight ? 0.40f : 1.00f);

    drawCircle(x - 0.06f * scale, y, 0.045f * scale, bR, bG, bB);
    drawCircle(x + 0.06f * scale, y, 0.045f * scale, bR, bG, bB);
    drawCircle(x, y + 0.025f * scale, 0.060f * scale, hR, hG, hB);
    drawCircle(x - 0.035f * scale, y + 0.012f * scale, 0.048f * scale, hR, hG, hB);
    drawCircle(x + 0.035f * scale, y + 0.015f * scale, 0.048f * scale, hR, hG, hB);
    drawRect(x - 0.06f * scale, y - 0.035f * scale, x + 0.06f * scale, y + 0.005f * scale, bR, bG, bB);
}

void drawSky() {
    if (isRaining) {
        if (isNight) {
            drawRect(-1, -0.28, 1, 1, 0.03, 0.04, 0.10);
            drawCircle(-0.8, 0.8, 0.075, 0.55, 0.55, 0.50);
        } else {
            drawRect(-1, -0.28, 1, 1, 0.42, 0.48, 0.55);
            drawCircle(-0.8, 0.8, 0.08, 0.80, 0.75, 0.50);
        }
    } else {
        if (isNight) {
            drawRect(-1, -0.28, 1, 1, 0.06, 0.08, 0.18);
            drawCircle(-0.8, 0.8, 0.08, 0.95, 0.95, 0.85);
        } else {
            drawRect(-1, -0.28, 1, 1, 0.55, 0.78, 0.98);
            drawCircle(-0.8, 0.8, 0.085, 1.0, 0.85, 0.20);
        }
    }
    drawCloud(cloud1X, 0.62f, 1.25f);
    drawCloud(cloud2X, 0.44f, 1.05f);
    drawCloud(cloud3X, 0.53f, 0.90f);
}

void drawNaturalTree(float x, float y, float scale) {
    drawRect(x - 0.015f * scale, y, x + 0.015f * scale, y + 0.30f * scale, 0.34f, 0.20f, 0.10f);
    drawLine(x, y + 0.18f * scale, x - 0.07f * scale, y + 0.30f * scale, 0.34f, 0.20f, 0.10f, 3);
    drawLine(x, y + 0.20f * scale, x + 0.07f * scale, y + 0.32f * scale, 0.34f, 0.20f, 0.10f, 3);
    float dG = isNight ? 0.24f : 0.43f, mG = isNight ? 0.34f : 0.58f, lG = isNight ? 0.43f : 0.70f;
    drawCircle(x - 0.065f * scale, y + 0.31f * scale, 0.065f * scale, 0.07f, dG, 0.12f);
    drawCircle(x + 0.065f * scale, y + 0.32f * scale, 0.065f * scale, 0.07f, dG, 0.12f);
    drawCircle(x - 0.045f * scale, y + 0.40f * scale, 0.075f * scale, 0.08f, mG, 0.14f);
    drawCircle(x + 0.045f * scale, y + 0.41f * scale, 0.075f * scale, 0.08f, mG, 0.14f);
    drawCircle(x, y + 0.49f * scale, 0.080f * scale, 0.10f, lG, 0.17f);
}

void drawStreetLamp(float x, float y, float height, bool faceRight) {
    drawRect(x - 0.014f, y, x + 0.014f, y + 0.035f, 0.25f, 0.25f, 0.28f);
    float aDir = faceRight ? 0.12f : -0.12f;
    drawLine(x, y, x, y + height, 0.15f, 0.15f, 0.18f, 4.5f);
    drawLine(x, y + height, x + aDir, y + height + 0.02f, 0.15f, 0.15f, 0.18f, 4.5f);
    drawLine(x + aDir, y + height + 0.02f, x + aDir, y + height - 0.025f, 0.15f, 0.15f, 0.18f, 4.5f);
    glColor3f(0.10f, 0.12f, 0.15f);
    glBegin(GL_POLYGON);
    glVertex2f(x + aDir - 0.035f, y + height - 0.015f);
    glVertex2f(x + aDir + 0.035f, y + height - 0.015f);
    glVertex2f(x + aDir + 0.022f, y + height + 0.020f);
    glVertex2f(x + aDir - 0.022f, y + height + 0.020f);
    glEnd();
    if (isNight || isRaining) drawCircle(x + aDir, y + height - 0.038f, 0.040f, 1.0f, 0.90f, 0.35f);
    drawCircle(x + aDir, y + height - 0.035f, 0.020f, 1.0f, 0.95f, 0.45f);
}

// ==========================================
// AIRPORT BUILDINGS & INFRASTRUCTURE
// ==========================================
void drawAirplane(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glColor3f(0.72f, 0.75f, 0.82f);
    glBegin(GL_POLYGON); glVertex2f(-0.04f, 0.018f); glVertex2f(0.05f, 0.018f); glVertex2f(0.0f, 0.11f); glVertex2f(-0.05f, 0.11f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-0.04f, -0.018f); glVertex2f(0.05f, -0.018f); glVertex2f(0.0f, -0.11f); glVertex2f(-0.05f, -0.11f); glEnd();
    glColor3f(0.68f, 0.70f, 0.76f);
    glBegin(GL_POLYGON); glVertex2f(-0.13f, 0.012f); glVertex2f(-0.08f, 0.012f); glVertex2f(-0.11f, 0.045f); glVertex2f(-0.14f, 0.045f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-0.13f, -0.012f); glVertex2f(-0.08f, -0.012f); glVertex2f(-0.11f, -0.045f); glVertex2f(-0.14f, -0.045f); glEnd();
    drawRect(-0.14f, -0.02f, 0.13f, 0.02f, 0.94f, 0.95f, 0.98f);
    glColor3f(0.94f, 0.95f, 0.98f);
    glBegin(GL_POLYGON); glVertex2f(0.13f, -0.02f); glVertex2f(0.18f, -0.006f); glVertex2f(0.18f, 0.006f); glVertex2f(0.13f, 0.02f); glEnd();
    drawRect(-0.14f, -0.003f, 0.14f, 0.003f, 0.85f, 0.20f, 0.20f);
    glColor3f(0.20f, 0.40f, 0.65f);
    glBegin(GL_POLYGON); glVertex2f(0.12f, 0.004f); glVertex2f(0.155f, 0.004f); glVertex2f(0.145f, 0.016f); glVertex2f(0.12f, 0.016f); glEnd();
    for (float px = -0.08f; px <= 0.09f; px += 0.022f) drawCircle(px, 0.008f, 0.004f, 0.25f, 0.45f, 0.70f);
    glColor3f(0.85f, 0.20f, 0.20f);
    glBegin(GL_POLYGON); glVertex2f(-0.14f, 0.02f); glVertex2f(-0.08f, 0.02f); glVertex2f(-0.13f, 0.075f); glVertex2f(-0.16f, 0.075f); glEnd();
    glPopMatrix();
}

void drawTerminal1And2Building() {
    float cR = 0.86f, cG = 0.80f, cB = 0.68f;
    float gR = isNight ? 0.18f : 0.55f, gG = isNight ? 0.32f : 0.68f, gB = isNight ? 0.44f : 0.78f;
    const float xL = -0.97f, xR = -0.62f, yB = -0.28f, yT = 0.04f;

    drawRect(xL, yB, xR, yT, cR, cG, cB);
    glColor3f(cR * 0.82f, cG * 0.82f, cB * 0.82f);
    glBegin(GL_POLYGON);
    glVertex2f(xR, yB); glVertex2f(xR + 0.045f, yB + 0.02f);
    glVertex2f(xR + 0.045f, yT + 0.02f); glVertex2f(xR, yT);
    glEnd();
    drawRect(xL, yT, xR + 0.045f, yT + 0.022f, 0.92f, 0.87f, 0.76f);

    float cx = xL + 0.15f, cy = yB + 0.20f, rad = 0.095f;
    drawCircle(cx, cy, rad, gR, gG, gB);
    glColor3f(0.28f, 0.26f, 0.22f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i += 10) {
        float a = i * 3.14159265f / 180.0f;
        glVertex2f(cx + cos(a) * rad, cy + sin(a) * rad);
    }
    glEnd();
    float offsets[3] = { -0.5f * rad, 0.0f, 0.5f * rad };
    for (int i = 0; i < 3; i++) {
        float dx = offsets[i], dy = offsets[i];
        drawLine(cx + dx, cy - sqrtf(rad * rad - dx * dx), cx + dx, cy + sqrtf(rad * rad - dx * dx), 0.28f, 0.26f, 0.22f, 1.5f);
        drawLine(cx - sqrtf(rad * rad - dy * dy), cy + dy, cx + sqrtf(rad * rad - dy * dy), cy + dy, 0.28f, 0.26f, 0.22f, 1.5f);
    }
}

void drawControlTower() {
    float cR = isNight ? 0.18f : 0.42f, cG = isNight ? 0.24f : 0.50f, cB = isNight ? 0.30f : 0.56f;
    float cpR = isNight ? 0.30f : 0.72f, cpG = isNight ? 0.36f : 0.78f, cpB = isNight ? 0.42f : 0.82f;
    const float xL = -0.62f, xR = -0.42f;

    drawRect(xL, -0.28f, xR, -0.12f, 0.86f, 0.80f, 0.68f);
    glColor3f(0.88f, 0.85f, 0.80f);
    glBegin(GL_POLYGON);
    glVertex2f(xL + 0.045f, -0.12f); glVertex2f(xR - 0.045f, -0.12f);
    glVertex2f(xR - 0.060f, 0.52f); glVertex2f(xL + 0.060f, 0.52f);
    glEnd();
    drawRect(xL + 0.01f, 0.30f, xR - 0.01f, 0.42f, cR, cG, cB);
    for (float wx = xL + 0.035f; wx <= xR - 0.05f; wx += 0.045f)
        drawRect(wx, 0.335f, wx + 0.025f, 0.385f, 0.10f, 0.13f, 0.18f);
    drawRect(xL + 0.055f, 0.52f, xR - 0.055f, 0.56f, cpR, cpG, cpB);
    drawCircle((xL + xR) * 0.5f, 0.575f, 0.045f, cpR, cpG, cpB);
}

void drawTerminal() {
    // Made slightly shorter vertically to simulate realistic distance/perspective from the road
    float r = isNight ? 0.30f : 0.82f, g = isNight ? 0.33f : 0.80f, b = isNight ? 0.39f : 0.76f;
    drawRect(0.02f, -0.28f, 0.88f, 0.16f, r, g, b);
    drawRect(0.02f, -0.28f, 0.88f, -0.10f, 0.25f, 0.27f, 0.30f);
    drawRect(0.07f, -0.08f, 0.83f, 0.12f, isNight ? 0.10f : 0.28f, isNight ? 0.28f : 0.62f, isNight ? 0.42f : 0.82f);

    for (float x = 0.07f; x <= 0.83f; x += 0.075f) drawLine(x, -0.08f, x, 0.12f, 0.12f, 0.14f, 0.17f, 1.5f);
    for (float y = -0.08f; y <= 0.12f; y += 0.055f) drawLine(0.07f, y, 0.83f, y, 0.12f, 0.14f, 0.17f, 1.5f);

    drawRect(-0.02f, 0.16f, 0.91f, 0.21f, 0.24f, 0.26f, 0.29f);
    drawRect(0.02f, 0.21f, 0.88f, 0.25f, 0.62f, 0.64f, 0.67f);

    for (int i = 0; i < 5; i++) {
        float x = 0.13f + i * 0.15f;
        drawRect(x - 0.018f, -0.28f, x + 0.018f, 0.09f, 0.76f, 0.77f, 0.79f);
        drawRect(x - 0.055f, -0.28f, x + 0.055f, 0.07f, isNight ? 0.10f : 0.25f, isNight ? 0.28f : 0.60f, isNight ? 0.40f : 0.78f);
        drawCircle(x, 0.07f, 0.045f, 0.72f, 0.74f, 0.77f);
        drawRect(x - 0.045f, 0.03f, x + 0.045f, 0.08f, isNight ? 0.10f : 0.25f, isNight ? 0.28f : 0.60f, isNight ? 0.40f : 0.78f);
    }
    drawRect(0.34f, 0.14f, 0.63f, 0.19f, 0.04f, 0.25f, 0.46f);
    drawRect(0.37f, 0.155f, 0.60f, 0.180f, 0.07f, 0.43f, 0.66f);

    // Text signage: Upper text in Red, Terminal name in Green
    drawText(0.12f, 0.225f, 0.90f, 0.10f, 0.10f, "HAZRAT SHAHJALAL INTERNATIONAL AIRPORT");
    drawText(0.39f, 0.162f, 0.10f, 0.75f, 0.20f, "TERMINAL 3");
}

// ==========================================
// ROAD, VEHICLES & WEATHER RENDERING
// ==========================================
void drawRoad() {
    drawRect(-1.0f, -0.36f, 1.0f, -0.28f, 0.58f, 0.58f, 0.60f);
    glColor3f(0.30f, 0.30f, 0.30f);
    glLineWidth(1.5f);
    for (float x = -1.0f; x <= 1.0f; x += 0.10f) {
        glBegin(GL_LINES); glVertex2f(x, -0.36f); glVertex2f(x, -0.28f); glEnd();
    }
    drawRect(-1.0f, -1.0f, 1.0f, -0.36f, 0.22f, 0.22f, 0.24f);
    drawRect(-1.0f, -0.69f, 1.0f, -0.675f, 0.90f, 0.75f, 0.10f);
    for (float x = -0.95f; x < 1.0f; x += 0.25f) {
        drawRect(x, -0.54f, x + 0.12f, -0.52f, 1.0f, 1.0f, 1.0f);
        drawRect(x, -0.86f, x + 0.12f, -0.84f, 1.0f, 1.0f, 1.0f);
    }
}

void drawRotatingWheel(float cx, float cy, float radius) {
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
    drawCircle(0, 0, radius, 0.10f, 0.10f, 0.11f);
    drawCircle(0, 0, radius * 0.45f, 0.70f, 0.70f, 0.72f);
    for (int i = 0; i < 4; i++) {
        float a = i * 3.14159265f / 2.0f;
        drawLine(0, 0, cos(a) * radius * 0.70f, sin(a) * radius * 0.70f, 0.18f, 0.18f, 0.20f, 1.5f);
    }
    glPopMatrix();
}

void drawCar(float x, float y, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    drawRect(-0.16f, -0.04f, 0.16f, 0.045f, r, g, b);
    glColor3f(r * 0.90f, g * 0.90f, b * 0.90f);
    glBegin(GL_POLYGON); glVertex2f(-0.10f, 0.045f); glVertex2f(0.08f, 0.045f); glVertex2f(0.05f, 0.11f); glVertex2f(-0.07f, 0.11f); glEnd();
    float wCol = isNight ? 0.35f : 0.75f;
    drawRect(-0.06f, 0.055f, -0.015f, 0.10f, wCol, wCol + 0.15f, 0.95f);
    drawRect(0.005f, 0.055f, 0.045f, 0.10f, wCol, wCol + 0.15f, 0.95f);
    drawRect(0.15f, 0.00f, 0.16f, 0.035f, 1.0f, 0.95f, 0.20f);
    drawRect(-0.16f, 0.00f, -0.15f, 0.035f, 0.90f, 0.10f, 0.10f);
    drawRotatingWheel(-0.09f, -0.04f, 0.035f);
    drawRotatingWheel(0.09f, -0.04f, 0.035f);
    glPopMatrix();
}

void drawBus(float x, float y, float r, float g, float b, float sR, float sG, float sB) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    drawRect(-0.30f, -0.05f, 0.30f, 0.17f, r, g, b);
    drawRect(-0.30f, 0.17f, 0.30f, 0.19f, r * 0.85f, g * 0.85f, b * 0.85f);
    drawRect(-0.30f, 0.025f, 0.30f, 0.065f, sR, sG, sB);
    float wCol = isNight ? 0.35f : 0.75f;
    for (int i = 0; i < 5; i++) {
        float wx = -0.29f + i * 0.095f;
        drawRect(wx, 0.065f, wx + 0.075f, 0.155f, wCol, wCol + 0.15f, 0.95f);
        glColor3f(0.15f, 0.15f, 0.18f); glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP); glVertex2f(wx, 0.065f); glVertex2f(wx + 0.075f, 0.065f); glVertex2f(wx + 0.075f, 0.155f); glVertex2f(wx, 0.155f); glEnd();
    }
    drawRect(0.20f, 0.065f, 0.30f, 0.155f, wCol, wCol + 0.15f, 0.95f);
    drawRect(0.30f, -0.02f, 0.32f, 0.035f, 1.0f, 0.95f, 0.20f);
    drawRect(-0.32f, -0.02f, -0.30f, 0.035f, 0.90f, 0.10f, 0.10f);
    drawRotatingWheel(-0.20f, -0.05f, 0.048f);
    drawRotatingWheel(0.20f, -0.05f, 0.048f);
    glPopMatrix();
}

void drawRain() {
    if (!isRaining) return;
    glColor3f(0.75f, 0.85f, 0.98f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (int i = 0; i < MAX_RAIN; i++) {
        glVertex2f(rainX[i], rainY[i]);
        glVertex2f(rainX[i] - 0.012f, rainY[i] - 0.045f);
    }
    glEnd();
}

// ==========================================
// DISPLAY & CALLBACK FUNCTIONS
// ==========================================








// =============================================================
// SCENE 1 SPECIFIC FUNCTIONS
// =============================================================

void drawDetailedCloudScene1(float cx, float cy, float scale) {
    float baseR, baseG, baseB, highR, highG, highB;

    if (isRaining) {
        baseR = isNight ? 0.10f : 0.40f;
        baseG = isNight ? 0.12f : 0.43f;
        baseB = isNight ? 0.16f : 0.48f;
        highR = isNight ? 0.15f : 0.50f;
        highG = isNight ? 0.17f : 0.53f;
        highB = isNight ? 0.22f : 0.58f;
    } else {
        baseR = isNight ? 0.20f : 0.90f;
        baseG = isNight ? 0.24f : 0.92f;
        baseB = isNight ? 0.32f : 0.96f;
        highR = isNight ? 0.28f : 1.00f;
        highG = isNight ? 0.32f : 1.00f;
        highB = isNight ? 0.40f : 1.00f;
    }

    drawCircle(cx - 0.06f * scale, cy - 0.015f * scale, 0.038f * scale, baseR, baseG, baseB);
    drawCircle(cx + 0.06f * scale, cy - 0.015f * scale, 0.038f * scale, baseR, baseG, baseB);
    drawRect(cx - 0.06f * scale, cy - 0.035f * scale, cx + 0.06f * scale, cy + 0.005f * scale, baseR, baseG, baseB);

    drawCircle(cx - 0.05f * scale, cy, 0.045f * scale, baseR, baseG, baseB);
    drawCircle(cx + 0.05f * scale, cy + 0.005f * scale, 0.042f * scale, baseR, baseG, baseB);
    drawCircle(cx, cy + 0.022f * scale, 0.058f * scale, highR, highG, highB);
    drawCircle(cx - 0.025f * scale, cy + 0.012f * scale, 0.048f * scale, highR, highG, highB);
    drawCircle(cx + 0.025f * scale, cy + 0.015f * scale, 0.046f * scale, highR, highG, highB);
}

void drawTerminalBackgroundTreeScene1(float x, float y, float scale) {
    drawRect(x - 0.020f * scale, y, x + 0.020f * scale, y + 0.58f * scale, 0.38f, 0.22f, 0.12f);

    glColor3f(0.38f, 0.22f, 0.12f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(x, y + 0.38f * scale);
        glVertex2f(x - 0.06f * scale, y + 0.52f * scale);
        glVertex2f(x, y + 0.42f * scale);
        glVertex2f(x + 0.06f * scale, y + 0.55f * scale);
    glEnd();

    float darkR = isNight ? 0.05f : 0.10f;
    float darkG = isNight ? 0.22f : 0.42f;
    float darkB = isNight ? 0.08f : 0.16f;

    float midR  = isNight ? 0.08f : 0.16f;
    float midG  = isNight ? 0.32f : 0.58f;
    float midB  = isNight ? 0.12f : 0.22f;

    float lightR = isNight ? 0.12f : 0.25f;
    float lightG = isNight ? 0.42f : 0.70f;
    float lightB = isNight ? 0.16f : 0.28f;

    drawCircle(x - 0.065f * scale, y + 0.50f * scale, 0.065f * scale, darkR, darkG, darkB);
    drawCircle(x + 0.065f * scale, y + 0.50f * scale, 0.065f * scale, darkR, darkG, darkB);
    drawCircle(x - 0.050f * scale, y + 0.60f * scale, 0.070f * scale, midR, midG, midB);
    drawCircle(x + 0.050f * scale, y + 0.60f * scale, 0.070f * scale, midR, midG, midB);
    drawCircle(x, y + 0.54f * scale, 0.075f * scale, midR, midG, midB);
    drawCircle(x - 0.030f * scale, y + 0.68f * scale, 0.062f * scale, lightR, lightG, lightB);
    drawCircle(x + 0.030f * scale, y + 0.68f * scale, 0.062f * scale, lightR, lightG, lightB);
    drawCircle(x, y + 0.76f * scale, 0.070f * scale, lightR, lightG, lightB);
}

void drawNaturalTreeScene1(float x, float y, float scale) {
    drawRect(x - 0.012f * scale, y, x + 0.012f * scale, y + 0.35f * scale, 0.38f, 0.22f, 0.12f);

    float darkR = isNight ? 0.05f : 0.12f;
    float darkG = isNight ? 0.25f : 0.45f;
    float darkB = isNight ? 0.10f : 0.18f;

    float midR  = isNight ? 0.08f : 0.18f;
    float midG  = isNight ? 0.35f : 0.60f;
    float midB  = isNight ? 0.14f : 0.24f;

    float lightR = isNight ? 0.12f : 0.28f;
    float lightG = isNight ? 0.45f : 0.72f;
    float lightB = isNight ? 0.18f : 0.30f;

    drawCircle(x - 0.045f * scale, y + 0.30f * scale, 0.048f * scale, darkR, darkG, darkB);
    drawCircle(x + 0.045f * scale, y + 0.30f * scale, 0.048f * scale, darkR, darkG, darkB);
    drawCircle(x - 0.035f * scale, y + 0.38f * scale, 0.050f * scale, midR, midG, midB);
    drawCircle(x + 0.035f * scale, y + 0.38f * scale, 0.050f * scale, midR, midG, midB);
    drawCircle(x, y + 0.44f * scale, 0.055f * scale, lightR, lightG, lightB);
}

void drawMainTerminalScene1() {
    float baseCol = isNight ? 0.32f : 0.88f;
    float baseColG = isNight ? 0.35f : 0.84f;
    float baseColB = isNight ? 0.40f : 0.78f;
    drawRect(-0.90f, -0.28f, -0.46f, 0.56f, baseCol, baseColG, baseColB);

    float pierCol = isNight ? 0.36f : 0.92f;
    float pierColG = isNight ? 0.39f : 0.88f;
    float pierColB = isNight ? 0.44f : 0.82f;
    drawRect(-0.91f, -0.28f, -0.87f, 0.58f, pierCol, pierColG, pierColB);
    drawRect(-0.49f, -0.28f, -0.45f, 0.58f, pierCol, pierColG, pierColB);

    float glassR = isNight ? 0.18f : 0.50f;
    float glassG = isNight ? 0.38f : 0.75f;
    float glassB = isNight ? 0.55f : 0.90f;
    drawRect(-0.86f, -0.05f, -0.50f, 0.52f, glassR, glassG, glassB);

    glColor3f(0.12f, 0.15f, 0.20f);
    glLineWidth(2.0f);
    for (float gx = -0.86f; gx <= -0.50f; gx += 0.06f) {
        glBegin(GL_LINES);
            glVertex2f(gx, -0.05f);
            glVertex2f(gx, 0.52f);
        glEnd();
    }
    for (float gy = -0.05f; gy <= 0.52f; gy += 0.071f) {
        glBegin(GL_LINES);
            glVertex2f(-0.86f, gy);
            glVertex2f(-0.50f, gy);
        glEnd();
    }

    if (!isNight && !isRaining) {
        glColor3f(0.85f, 0.95f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-0.82f, 0.48f); glVertex2f(-0.72f, 0.15f);
            glVertex2f(-0.78f, 0.48f); glVertex2f(-0.68f, 0.15f);
            glVertex2f(-0.64f, 0.48f); glVertex2f(-0.54f, 0.15f);
        glEnd();
    }

    glColor3f(0.28f, 0.30f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.43f, 0.56f);
        glVertex2f(-0.92f, 0.56f);
        glVertex2f(-0.98f, 0.72f);
        glVertex2f(-0.45f, 0.64f);
    glEnd();

    glColor3f(0.85f, 0.40f, 0.15f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.92f, 0.56f);
        glVertex2f(-0.98f, 0.72f);
        glVertex2f(-0.96f, 0.73f);
        glVertex2f(-0.90f, 0.58f);
    glEnd();

    glColor3f(0.20f, 0.20f, 0.24f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(-0.90f, 0.56f); glVertex2f(-0.96f, 0.68f);
        glVertex2f(-0.78f, 0.56f); glVertex2f(-0.84f, 0.68f);
        glVertex2f(-0.66f, 0.56f); glVertex2f(-0.72f, 0.68f);
        glVertex2f(-0.54f, 0.56f); glVertex2f(-0.60f, 0.68f);
    glEnd();

    glColor3f(0.25f, 0.25f, 0.28f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(-0.68f, 0.60f); glVertex2f(-0.68f, 0.78f);
        glVertex2f(-0.71f, 0.73f); glVertex2f(-0.65f, 0.73f);
    glEnd();
    drawCircle(-0.68f, 0.79f, 0.012f, 1.0f, 0.1f, 0.1f);
}

void drawMiddleBuildingScene1() {
    float bR = isNight ? 0.32f : 0.88f;
    float bG = isNight ? 0.35f : 0.84f;
    float bB = isNight ? 0.40f : 0.78f;

    drawRect(-0.44f, -0.28f, 0.00f, 0.22f, bR, bG, bB);
    drawRect(-0.45f, 0.22f, 0.01f, 0.24f, 0.35f, 0.35f, 0.38f);

    float winR = isNight ? 0.98f : 0.20f;
    float winG = isNight ? 0.85f : 0.40f;
    float winB = isNight ? 0.20f : 0.65f;

    for (int i = 0; i < 4; i++) {
        float wx = -0.41f + i * 0.10f;
        drawRect(wx, -0.02f, wx + 0.06f, 0.08f, winR, winG, winB);

        glColor3f(0.20f, 0.20f, 0.20f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(wx, -0.02f);
            glVertex2f(wx + 0.06f, -0.02f);
            glVertex2f(wx + 0.06f, 0.08f);
            glVertex2f(wx, 0.08f);
        glEnd();
    }
}

void drawBackgroundTreesScene1() {
    drawNaturalTreeScene1(0.20f, -0.28f, 0.85f);
    drawNaturalTreeScene1(0.38f, -0.28f, 0.95f);
    drawNaturalTreeScene1(0.54f, -0.28f, 0.90f);
    drawNaturalTreeScene1(0.82f, -0.28f, 1.15f);
}

void drawConcretePillarsScene1() {
    float heights[6] = {0.20f, 0.25f, 0.30f, 0.35f, 0.40f, 0.46f};
    float startX = 0.09f;
    float width = 0.075f;
    float gap = 0.02f;

    for (int i = 0; i < 6; i++) {
        float x1 = startX + i * (width + gap);
        float x2 = x1 + width;
        float topY = -0.28f + heights[i];

        float baseAsh = isNight ? 0.24f : 0.55f;
        float ashVal = baseAsh + i * 0.025f;
        drawRect(x1, -0.28f, x2, topY, ashVal, ashVal + 0.01f, ashVal + 0.02f);

        glColor3f(0.18f, 0.18f, 0.20f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x1, -0.28f);
            glVertex2f(x2, -0.28f);
            glVertex2f(x2, topY);
            glVertex2f(x1, topY);
        glEnd();

        drawRect(x1 + 0.015f, topY - 0.04f, x2 - 0.015f, topY - 0.02f, 0.20f, 0.20f, 0.22f);
    }
}

void drawDirectionBoardScene1() {
    drawRect(0.70f, -0.28f, 0.72f, 0.00f, 0.30f, 0.30f, 0.30f);
    drawRect(0.92f, -0.28f, 0.94f, 0.00f, 0.30f, 0.30f, 0.30f);

    drawRect(0.68f, 0.00f, 0.96f, 0.24f, 0.08f, 0.52f, 0.22f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.69f, 0.01f);
        glVertex2f(0.95f, 0.01f);
        glVertex2f(0.95f, 0.23f);
        glVertex2f(0.69f, 0.23f);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(0.82f, 0.01f);
        glVertex2f(0.82f, 0.23f);
    glEnd();

    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glVertex2f(0.72f, 0.12f);
        glVertex2f(0.79f, 0.12f);
        glVertex2f(0.79f, 0.12f);
        glVertex2f(0.76f, 0.15f);
        glVertex2f(0.79f, 0.12f);
        glVertex2f(0.76f, 0.09f);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(0.85f, 0.12f);
        glVertex2f(0.92f, 0.12f);
        glVertex2f(0.92f, 0.12f);
        glVertex2f(0.89f, 0.15f);
        glVertex2f(0.92f, 0.12f);
        glVertex2f(0.89f, 0.09f);
    glEnd();
}

void drawRoadAndDividersScene1() {
    drawRect(-1.0f, -0.36f, 1.0f, -0.28f, 0.58f, 0.58f, 0.60f);
    glColor3f(0.30f, 0.30f, 0.30f);
    glLineWidth(1.5f);
    for (float fx = -1.0f; fx <= 1.0f; fx += 0.10f) {
        glBegin(GL_LINES);
            glVertex2f(fx, -0.36f);
            glVertex2f(fx, -0.28f);
        glEnd();
    }

    drawRect(-1.0f, -1.0f, 1.0f, -0.36f, 0.22f, 0.22f, 0.24f);

    glColor3f(1.0f, 1.0f, 1.0f);
    for (float rx = -0.95f; rx < 1.0f; rx += 0.25f) {
        drawRect(rx, -0.675f, rx + 0.12f, -0.655f, 1.0f, 1.0f, 1.0f);
    }
}

void drawRotatingWheelScene1(float cx, float cy, float radius) {
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.12f, 0.12f, 0.12f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 15) {
        float rad = i * 3.14159265f / 180.0f;
        glVertex2f(cos(rad) * radius, sin(rad) * radius);
    }
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(0.85f, 0.20f, 0.20f);
        for (float t = 0; t <= 3.14159f; t += 0.15f) {
            glVertex2f(cos(t) * (radius * 0.6f), sin(t) * (radius * 0.6f));
        }
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(0.95f, 0.95f, 0.95f);
        for (float t = 3.14159f; t <= 2 * 3.14159f; t += 0.15f) {
            glVertex2f(cos(t) * (radius * 0.6f), sin(t) * (radius * 0.6f));
        }
    glEnd();

    glPopMatrix();
}

void drawCarScene1(float x, float y, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    drawRect(-0.16f, -0.04f, 0.16f, 0.045f, r, g, b);

    glColor3f(r * 0.9f, g * 0.9f, b * 0.9f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.10f, 0.045f);
        glVertex2f(0.08f, 0.045f);
        glVertex2f(0.05f, 0.11f);
        glVertex2f(-0.07f, 0.11f);
    glEnd();

    float winCol = isNight ? 0.35f : 0.75f;
    drawRect(-0.06f, 0.055f, -0.015f, 0.10f, winCol, winCol + 0.15f, 0.95f);
    drawRect(0.005f, 0.055f, 0.045f, 0.10f, winCol, winCol + 0.15f, 0.95f);

    drawRect(0.15f, 0.00f, 0.16f, 0.035f, 1.0f, 0.95f, 0.2f);
    drawRect(-0.16f, 0.00f, -0.15f, 0.035f, 0.9f, 0.1f, 0.1f);

    drawRotatingWheelScene1(-0.09f, -0.04f, 0.035f);
    drawRotatingWheelScene1(0.09f, -0.04f, 0.035f);

    glPopMatrix();
}

void drawBusScene1(float x, float y, float r, float g, float b, float stripeR, float stripeG, float stripeB) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    drawRect(-0.32f, -0.05f, 0.32f, 0.17f, r, g, b);
    drawRect(-0.32f, 0.17f, 0.32f, 0.19f, r * 0.85f, g * 0.85f, b * 0.85f);
    drawRect(-0.32f, 0.025f, 0.32f, 0.065f, stripeR, stripeG, stripeB);

    float winCol = isNight ? 0.35f : 0.75f;
    drawRect(0.20f, 0.065f, 0.30f, 0.155f, winCol, winCol + 0.15f, 0.95f);

    glColor3f(0.15f, 0.15f, 0.18f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.20f, 0.065f);
        glVertex2f(0.30f, 0.065f);
        glVertex2f(0.30f, 0.155f);
        glVertex2f(0.20f, 0.155f);
    glEnd();

    for (int i = 0; i < 5; i++) {
        float wx = -0.29f + i * 0.095f;
        drawRect(wx, 0.065f, wx + 0.075f, 0.155f, winCol, winCol + 0.15f, 0.95f);

        glColor3f(0.15f, 0.15f, 0.18f);
        glLineWidth(1.8f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(wx, 0.065f);
            glVertex2f(wx + 0.075f, 0.065f);
            glVertex2f(wx + 0.075f, 0.155f);
            glVertex2f(wx, 0.155f);
        glEnd();
    }

    drawRect(0.315f, -0.02f, 0.32f, 0.035f, 1.0f, 0.95f, 0.2f);
    drawRect(-0.32f, -0.02f, -0.315f, 0.035f, 0.9f, 0.1f, 0.1f);

    drawRotatingWheelScene1(-0.20f, -0.05f, 0.048f);
    drawRotatingWheelScene1(0.20f, -0.05f, 0.048f);

    glPopMatrix();
}

void displayScene1() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();
    drawAirplane(planeX, 0.80f);
drawTerminalBackgroundTreeScene1();
drawBackgroundTreesScene1();
drawMainTerminalScene1();
drawMiddleBuildingScene1();
drawConcretePillarsScene1();
drawDirectionBoardScene1();
drawRoadAndDividersScene1();

    drawCarScene1(car1X, -0.48f, 0.85f, 0.20f, 0.20f);
    drawCarScene1(car2X, -0.48f, 0.20f, 0.55f, 0.85f);
    drawBusScene1(bus1X, -0.48f, 0.10f, 0.60f, 0.65f, 1.0f, 0.85f, 0.20f);

    drawCarScene1(car3X, -0.82f, 0.88f, 0.65f, 0.15f);
    drawBusScene1(bus2X, -0.82f, 0.90f, 0.40f, 0.15f, 0.95f, 0.95f, 0.95f);

    drawRain();

    glFlush();
}

void displayScene2() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawSky();

    Point2D p0 = {-1.4f, 0.70f};
    Point2D p1 = {-0.5f, 0.90f};
    Point2D p2 = { 0.5f, 0.65f};
    Point2D p3 = { 1.4f, 0.80f};
    Point2D planePos = calculateBezierPoint(planeParam, p0, p1, p2, p3);

    drawAirplane(planePos.x, planePos.y);

    drawNaturalTree(-0.96f, -0.28f, 1.05f);
    drawNaturalTree(0.92f, -0.28f, 1.10f);
    drawNaturalTree(0.82f, -0.28f, 0.80f);
    drawTerminal1And2Building();
    drawControlTower();
    drawTerminal();

    drawConstructionCrane(-0.25f, -0.28f);
    drawConstructionCrane(0.55f, -0.28f);

    drawStreetLamp(-0.43f, -0.28f, 0.58f, false);
    drawStreetLamp(0.05f, -0.28f, 0.54f, false);
    drawStreetLamp(0.63f, -0.28f, 0.46f, false);

    drawRoad();
    drawBlueFence();

    drawCar(car1X, -0.42f, 0.85f, 0.20f, 0.20f);
    drawCar(car2X, -0.42f, 0.20f, 0.55f, 0.85f);
    drawBus(bus1X, -0.42f, 0.10f, 0.60f, 0.65f, 1.0f, 0.85f, 0.20f);
    drawCar(car3X, -0.82f, 0.88f, 0.65f, 0.15f);
    drawBus(bus2X, -0.82f, 0.90f, 0.40f, 0.15f, 0.95f, 0.95f, 0.95f);
    drawRain();
    glFlush();
}

// =============================================================
// UNIFIED ANIMATION TIMER
// =============================================================
void timer(int value) {
    if (!isPaused) {
        car1X += 0.008f;
        car2X += 0.008f;
        car3X += 0.010f;
        if (car1X > 1.5f) car1X = -1.5f;
        if (car2X > 1.5f) car2X = -1.5f;
        if (car3X > 1.5f) car3X = -1.5f;

        bus1X += 0.0065f;
        bus2X += 0.0075f;
        if (bus1X > 1.5f) bus1X = -1.5f;
        if (bus2X > 1.5f) bus2X = -1.5f;

        wheelAngle -= 8.0f;
        if (wheelAngle <= -360.0f) wheelAngle = 0.0f;

        planeX += 0.0035f;
        if (planeX > 1.4f) planeX = -1.4f;

        planeParam += 0.004f;
        if (planeParam > 1.0f) planeParam = 0.0f;

        cloud1X += 0.0010f;
        if (cloud1X > 1.4f) cloud1X = -1.4f;

        cloud2X += 0.0006f;
        if (cloud2X > 1.4f) cloud2X = -1.4f;

        cloud3X += 0.0014f;
        if (cloud3X > 1.4f) cloud3X = -1.4f;

        if (isRaining) {
            for (int i = 0; i < MAX_RAIN; i++) {
                rainY[i] -= rainSpeed[i];
                rainX[i] -= 0.003f;

                if (rainY[i] < -1.0f) {
                    rainY[i] = 1.0f;
                    rainX[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
                }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}


// =============================================================
// KEYBOARD CONTROLS
// =============================================================
void keyboard(unsigned char key, int x, int y) {
    if (key == '1') {
        currentScene = 1;
    }
    else if (key == '2') {
        currentScene = 2;
    }
    else if (key == 'n' || key == 'N') {
        isNight = true;
    }
    else if (key == 'd' || key == 'D') {
        isNight = false;
    }
    else if (key == 'r' || key == 'R') {
        isRaining = !isRaining;
    }
    else if (key == 'p' || key == 'P' || key == ' ') {
        isPaused = !isPaused;
    }

    glutPostRedisplay();
}


// =============================================================
// DISPLAY SWITCHER
// =============================================================
void display() {
    if (currentScene == 1)
        displayScene1();
    else
        displayScene2();
}

// =============================================================
// MAIN
// =============================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);

    glutCreateWindow(
        "Hazrat Shahjalal International Airport - Merged Scenes"
    );

    initRain();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
