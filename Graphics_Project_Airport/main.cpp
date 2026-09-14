#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdio>

// ==========================================
// GLOBAL VARIABLES & SCENE MANAGEMENT
// ==========================================
int currentScene = 1; // 1 = Scene 1, 2 = Scene 2, 3 = Scene 3

bool isNight = false;
bool isRaining = false;
bool isFoggy = false;
bool isPaused = false;

// Shared Global Animation Variables
float car1X = -1.30f, car2X = -0.65f, car3X = -0.95f;
float bus1X = 0.15f, bus2X = -0.35f;
float wheelAngle = 0.0f;
float cloud1X = -0.70f, cloud2X = 0.20f, cloud3X = -1.20f;

// Scene 1 Specific
float planeX1 = -1.40f;

// Scene 2 Specific
float planeParam2 = 0.0f;

// Scene 3 Specific
float planePosX3 = -1.40f;
float fireTruckX3 = -1.30f;
float sportsCarX3 = -1.50f;
float luxurySUV_X3 = 0.40f;
float cargoVanX3 = -0.70f;
float radarAngle3 = 0.0f;
bool emergencySiren3 = false;
int sirenTicks3 = 0;
bool beaconState3 = true;
int beaconTicks3 = 0;

const int MAX_RAIN = 160;
float rainX[MAX_RAIN], rainY[MAX_RAIN], rainSpeed[MAX_RAIN];

const float PI = 3.1415926535f;

// ==========================================
// STRUCTS & UTILITIES
// ==========================================
struct Point2D {
    float x, y;
};

// ID: UTIL_BEZIER_CALC
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

// ID: ANIM_RAIN_SIM_06
void initRain() {
    for (int i = 0; i < MAX_RAIN; i++) {
        rainX[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        rainY[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        rainSpeed[i] = 0.025f + ((float)rand() / RAND_MAX) * 0.02f;
    }
}

// ID: UTIL_DRAW_RECT
void drawRect(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// ID: UTIL_SOLID_BOX
void drawSolidBox(float x1, float y1, float x2, float y2, float r, float g, float b, float alpha = 1.0f) {
    glColor4f(r, g, b, alpha);
    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

// ID: UTIL_DRAW_CIRCLE
void drawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 15) {
        float rad = i * 3.14159265f / 180.0f;
        glVertex2f(cx + cos(rad) * radius, cy + sin(rad) * radius);
    }
    glEnd();
}

// ID: UTIL_SMOOTH_CIRCLE
void drawSmoothCircle(float cx, float cy, float rad, float r, float g, float b, float alpha = 1.0f) {
    glColor4f(r, g, b, alpha);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float rads = i * PI / 180.0f;
        glVertex2f(cx + cos(rads) * rad, cy + sin(rads) * rad);
    }
    glEnd();
}

// ID: UTIL_DRAW_LINE
void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float width) {
    glColor3f(r, g, b);
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1.0f);
}

// ID: UTIL_DRAW_TEXT
void drawText(float x, float y, float r, float g, float b, const char* text) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// ID: UTIL_BITMAP_TEXT
void drawBitmapText(float x, float y, const char* str, void* font, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = str; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

// ==========================================
// SHARED PRIMITIVES
// ==========================================
// ID: ANIM_WHEEL_ROT_03
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

// ID: OBJ_SKY_01
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
    auto drawCloudPrv = [](float x, float y, float scale) {
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
    };
    drawCloudPrv(cloud1X, 0.62f, 1.25f);
    drawCloudPrv(cloud2X, 0.44f, 1.05f);
    drawCloudPrv(cloud3X, 0.53f, 0.90f);
}

// ID: OBJ_AIRPLANE_02
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

// ID: OBJ_CAR_11
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

// ID: OBJ_BUS_10
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

// ID: OBJ_RAIN_12
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

// ID: OBJ_STREET_LAMP_08
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
// SCENE 1 FUNCTIONS
// ==========================================
// ID: SCENE1_TREE_FUNC
void drawNaturalTree1(float x, float y, float scale) {
    drawRect(x - 0.012f * scale, y, x + 0.012f * scale, y + 0.35f * scale, 0.38f, 0.22f, 0.12f);
    float darkR = isNight ? 0.05f : 0.12f, darkG = isNight ? 0.25f : 0.45f, darkB = isNight ? 0.10f : 0.18f;
    float midR = isNight ? 0.08f : 0.18f, midG = isNight ? 0.35f : 0.60f, midB = isNight ? 0.14f : 0.24f;
    float lightR = isNight ? 0.12f : 0.28f, lightG = isNight ? 0.45f : 0.72f, lightB = isNight ? 0.18f : 0.30f;
    drawCircle(x - 0.045f * scale, y + 0.30f * scale, 0.048f * scale, darkR, darkG, darkB);
    drawCircle(x + 0.045f * scale, y + 0.30f * scale, 0.048f * scale, darkR, darkG, darkB);
    drawCircle(x - 0.035f * scale, y + 0.38f * scale, 0.050f * scale, midR, midG, midB);
    drawCircle(x + 0.035f * scale, y + 0.38f * scale, 0.050f * scale, midR, midG, midB);
    drawCircle(x, y + 0.44f * scale, 0.055f * scale, lightR, lightG, lightB);
}

// ID: SCENE1_BG_TREE
void drawTerminalBackgroundTree1(float x, float y, float scale) {
    drawRect(x - 0.020f * scale, y, x + 0.020f * scale, y + 0.58f * scale, 0.38f, 0.22f, 0.12f);
    glColor3f(0.38f, 0.22f, 0.12f); glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(x, y + 0.38f * scale); glVertex2f(x - 0.06f * scale, y + 0.52f * scale);
        glVertex2f(x, y + 0.42f * scale); glVertex2f(x + 0.06f * scale, y + 0.55f * scale);
    glEnd();
    float darkR = isNight ? 0.05f : 0.10f, darkG = isNight ? 0.22f : 0.42f, darkB = isNight ? 0.08f : 0.16f;
    float midR = isNight ? 0.08f : 0.16f, midG = isNight ? 0.32f : 0.58f, midB = isNight ? 0.12f : 0.22f;
    float lightR = isNight ? 0.12f : 0.25f, lightG = isNight ? 0.42f : 0.70f, lightB = isNight ? 0.16f : 0.28f;
    drawCircle(x - 0.065f * scale, y + 0.50f * scale, 0.065f * scale, darkR, darkG, darkB);
    drawCircle(x + 0.065f * scale, y + 0.50f * scale, 0.065f * scale, darkR, darkG, darkB);
    drawCircle(x - 0.050f * scale, y + 0.60f * scale, 0.070f * scale, midR, midG, midB);
    drawCircle(x + 0.050f * scale, y + 0.60f * scale, 0.070f * scale, midR, midG, midB);
    drawCircle(x, y + 0.54f * scale, 0.075f * scale, midR, midG, midB);
    drawCircle(x - 0.030f * scale, y + 0.68f * scale, 0.062f * scale, lightR, lightG, lightB);
    drawCircle(x + 0.030f * scale, y + 0.68f * scale, 0.062f * scale, lightR, lightG, lightB);
    drawCircle(x, y + 0.76f * scale, 0.070f * scale, lightR, lightG, lightB);
}

// ID: SCENE1_MAIN_TERM
void drawMainTerminal1() {
    float baseCol = isNight ? 0.32f : 0.88f;
    float baseColG = isNight ? 0.35f : 0.84f;
    float baseColB = isNight ? 0.40f : 0.78f;
    drawRect(-0.90f, -0.28f, -0.46f, 0.56f, baseCol, baseColG, baseColB);
    float pierCol = isNight ? 0.36f : 0.92f, pierColG = isNight ? 0.39f : 0.88f, pierColB = isNight ? 0.44f : 0.82f;
    drawRect(-0.91f, -0.28f, -0.87f, 0.58f, pierCol, pierColG, pierColB);
    drawRect(-0.49f, -0.28f, -0.45f, 0.58f, pierCol, pierColG, pierColB);
    float glassR = isNight ? 0.18f : 0.50f, glassG = isNight ? 0.38f : 0.75f, glassB = isNight ? 0.55f : 0.90f;
    drawRect(-0.86f, -0.05f, -0.50f, 0.52f, glassR, glassG, glassB);
    glColor3f(0.12f, 0.15f, 0.20f); glLineWidth(2.0f);
    for (float gx = -0.86f; gx <= -0.50f; gx += 0.06f) { glBegin(GL_LINES); glVertex2f(gx, -0.05f); glVertex2f(gx, 0.52f); glEnd(); }
    for (float gy = -0.05f; gy <= 0.52f; gy += 0.071f) { glBegin(GL_LINES); glVertex2f(-0.86f, gy); glVertex2f(-0.50f, gy); glEnd(); }
    if (!isNight && !isRaining) {
        glColor3f(0.85f, 0.95f, 1.0f); glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-0.82f, 0.48f); glVertex2f(-0.72f, 0.15f);
            glVertex2f(-0.78f, 0.48f); glVertex2f(-0.68f, 0.15f);
            glVertex2f(-0.64f, 0.48f); glVertex2f(-0.54f, 0.15f);
        glEnd();
    }
    glColor3f(0.28f, 0.30f, 0.35f);
    glBegin(GL_POLYGON); glVertex2f(-0.43f, 0.56f); glVertex2f(-0.92f, 0.56f); glVertex2f(-0.98f, 0.72f); glVertex2f(-0.45f, 0.64f); glEnd();
    glColor3f(0.85f, 0.40f, 0.15f);
    glBegin(GL_POLYGON); glVertex2f(-0.92f, 0.56f); glVertex2f(-0.98f, 0.72f); glVertex2f(-0.96f, 0.73f); glVertex2f(-0.90f, 0.58f); glEnd();
}

// ID: SCENE1_MID_BLDG
void drawMiddleBuilding1() {
    float bR = isNight ? 0.32f : 0.88f, bG = isNight ? 0.35f : 0.84f, bB = isNight ? 0.40f : 0.78f;
    drawRect(-0.44f, -0.28f, 0.00f, 0.22f, bR, bG, bB);
    drawRect(-0.45f, 0.22f, 0.01f, 0.24f, 0.35f, 0.35f, 0.38f);
    float winR = isNight ? 0.98f : 0.20f, winG = isNight ? 0.85f : 0.40f, winB = isNight ? 0.20f : 0.65f;
    for (int i = 0; i < 4; i++) {
        float wx = -0.41f + i * 0.10f;
        drawRect(wx, -0.02f, wx + 0.06f, 0.08f, winR, winG, winB);
    }
}

// ID: SCENE1_PILLARS
void drawConcretePillars1() {
    float heights[6] = {0.20f, 0.25f, 0.30f, 0.35f, 0.40f, 0.46f};
    float startX = 0.09f, width = 0.075f, gap = 0.02f;
    for (int i = 0; i < 6; i++) {
        float x1 = startX + i * (width + gap), x2 = x1 + width, topY = -0.28f + heights[i];
        float baseAsh = isNight ? 0.24f : 0.55f, ashVal = baseAsh + i * 0.025f;
        drawRect(x1, -0.28f, x2, topY, ashVal, ashVal + 0.01f, ashVal + 0.02f);
        drawRect(x1 + 0.015f, topY - 0.04f, x2 - 0.015f, topY - 0.02f, 0.20f, 0.20f, 0.22f);
    }
}

// ID: SCENE1_BOARD
void drawDirectionBoard1() {
    drawRect(0.70f, -0.28f, 0.72f, 0.00f, 0.30f, 0.30f, 0.30f);
    drawRect(0.92f, -0.28f, 0.94f, 0.00f, 0.30f, 0.30f, 0.30f);
    drawRect(0.68f, 0.00f, 0.96f, 0.24f, 0.08f, 0.52f, 0.22f);
}

// ID: SCENE1_ROAD
void drawRoadAndDividers1() {
    drawRect(-1.0f, -0.36f, 1.0f, -0.28f, 0.58f, 0.58f, 0.60f);
    glColor3f(0.30f, 0.30f, 0.30f); glLineWidth(1.5f);
    for (float fx = -1.0f; fx <= 1.0f; fx += 0.10f) { glBegin(GL_LINES); glVertex2f(fx, -0.36f); glVertex2f(fx, -0.28f); glEnd(); }
    drawRect(-1.0f, -1.0f, 1.0f, -0.36f, 0.22f, 0.22f, 0.24f);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (float rx = -0.95f; rx < 1.0f; rx += 0.25f) { drawRect(rx, -0.675f, rx + 0.12f, -0.655f, 1.0f, 1.0f, 1.0f); }
}

// ID: SCENE1_RENDER_FUNC
void drawScene1() {
    drawSky();
    drawAirplane(planeX1, 0.80f);
    drawTerminalBackgroundTree1(-0.94f, -0.28f, 1.15f);
    drawNaturalTree1(0.20f, -0.28f, 0.85f);
    drawNaturalTree1(0.38f, -0.28f, 0.95f);
    drawNaturalTree1(0.54f, -0.28f, 0.90f);
    drawNaturalTree1(0.82f, -0.28f, 1.15f);
    drawMainTerminal1();
    drawMiddleBuilding1();
    drawConcretePillars1();
    drawDirectionBoard1();
    drawStreetLamp(-0.44f, -0.28f, 0.64f, false);
    drawStreetLamp(0.06f, -0.28f, 0.58f, false);
    drawStreetLamp(0.64f, -0.28f, 0.44f, false);
    drawRoadAndDividers1();

    drawCar(car1X, -0.48f, 0.85f, 0.20f, 0.20f);
    drawCar(car2X, -0.48f, 0.20f, 0.55f, 0.85f);
    drawBus(bus1X, -0.48f, 0.10f, 0.60f, 0.65f, 1.0f, 0.85f, 0.20f);
    drawCar(car3X, -0.82f, 0.88f, 0.65f, 0.15f);
    drawBus(bus2X, -0.82f, 0.90f, 0.40f, 0.15f, 0.95f, 0.95f, 0.95f);
    drawRain();

    drawText(-0.95f, 0.90f, 1.0f, 1.0f, 1.0f, "SCENE 1 (Press '2' for Scene 2, '3' for Scene 3)");
}

// ==========================================
// SCENE 2 FUNCTIONS
// ==========================================
// ID: SCENE2_TREE_FUNC
void drawNaturalTree2(float x, float y, float scale) {
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

// ID: SCENE2_CRANE_FUNC
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

// ID: SCENE2_FENCE_FUNC
void drawBlueFence() {
    drawRect(-1.0f, -0.48f, 1.0f, -0.36f, 0.12f, 0.32f, 0.58f);
    for (float x = -1.0f; x <= 1.0f; x += 0.03f) {
        drawLine(x, -0.48f, x, -0.36f, 0.08f, 0.22f, 0.42f, 1.0f);
    }
}

// ID: SCENE2_TERM_1_2
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
}

// ID: SCENE2_TOWER_FUNC
void drawControlTower2() {
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
    drawRect(xL + 0.055f, 0.52f, xR - 0.055f, 0.56f, cpR, cpG, cpB);
    drawCircle((xL + xR) * 0.5f, 0.575f, 0.045f, cpR, cpG, cpB);
}

// ID: SCENE2_TERM_1
void drawTerminal2() {
    float r = isNight ? 0.30f : 0.82f, g = isNight ? 0.33f : 0.80f, b = isNight ? 0.39f : 0.76f;
    drawRect(0.02f, -0.28f, 0.88f, 0.16f, r, g, b);
    drawRect(0.02f, -0.28f, 0.88f, -0.10f, 0.25f, 0.27f, 0.30f);
    drawRect(0.07f, -0.08f, 0.83f, 0.12f, isNight ? 0.10f : 0.28f, isNight ? 0.28f : 0.62f, isNight ? 0.42f : 0.82f);
    for (float x = 0.07f; x <= 0.83f; x += 0.075f) drawLine(x, -0.08f, x, 0.12f, 0.12f, 0.14f, 0.17f, 1.5f);
    for (float y = -0.08f; y <= 0.12f; y += 0.055f) drawLine(0.07f, y, 0.83f, y, 0.12f, 0.14f, 0.17f, 1.5f);
    drawRect(-0.02f, 0.16f, 0.91f, 0.21f, 0.24f, 0.26f, 0.29f);
    drawRect(0.02f, 0.21f, 0.88f, 0.25f, 0.62f, 0.64f, 0.67f);
    drawText(0.12f, 0.225f, 0.90f, 0.10f, 0.10f, "HAZRAT SHAHJALAL INTERNATIONAL AIRPORT");
    drawText(0.39f, 0.162f, 0.10f, 0.75f, 0.20f, "TERMINAL 1");
}

// ID: SCENE2_ROAD_FUNC
void drawRoad2() {
    drawRect(-1.0f, -0.36f, 1.0f, -0.28f, 0.58f, 0.58f, 0.60f);
    glColor3f(0.30f, 0.30f, 0.30f); glLineWidth(1.5f);
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

// ID: SCENE2_RENDER_FUNC
void drawScene2() {
    drawSky();

    Point2D p0 = {-1.4f, 0.70f};
    Point2D p1 = {-0.5f, 0.90f};
    Point2D p2 = { 0.5f, 0.65f};
    Point2D p3 = { 1.4f, 0.80f};
    Point2D planePos = calculateBezierPoint(planeParam2, p0, p1, p2, p3);

    drawAirplane(planePos.x, planePos.y);

    drawNaturalTree2(-0.96f, -0.28f, 1.05f);
    drawNaturalTree2(0.92f, -0.28f, 1.10f);
    drawNaturalTree2(0.82f, -0.28f, 0.80f);
    drawTerminal1And2Building();
    drawControlTower2();
    drawTerminal2();

    drawConstructionCrane(-0.25f, -0.28f);
    drawConstructionCrane(0.55f, -0.28f);

    drawStreetLamp(-0.43f, -0.28f, 0.58f, false);
    drawStreetLamp(0.05f, -0.28f, 0.54f, false);
    drawStreetLamp(0.63f, -0.28f, 0.46f, false);

    drawRoad2();
    drawBlueFence();

    drawCar(car1X, -0.42f, 0.85f, 0.20f, 0.20f);
    drawCar(car2X, -0.42f, 0.20f, 0.55f, 0.85f);
    drawBus(bus1X, -0.42f, 0.10f, 0.60f, 0.65f, 1.0f, 0.85f, 0.20f);
    drawCar(car3X, -0.82f, 0.88f, 0.65f, 0.15f);
    drawBus(bus2X, -0.82f, 0.90f, 0.40f, 0.15f, 0.95f, 0.95f, 0.95f);
    drawRain();

    drawText(-0.95f, 0.90f, 1.0f, 1.0f, 1.0f, "SCENE 2 (Press '1' for Scene 1, '3' for Scene 3)");
}

// ==========================================
// SCENE 3 FUNCTIONS (With original IDs intact)
// ==========================================
// ID: SCENE3_ENV_SKY
void renderEnvironmentSky3() {
    if (isNight) {
        drawSolidBox(-1.0f, -0.25f, 1.0f, 1.0f, 0.02f, 0.03f, 0.07f);
        drawSmoothCircle(0.70f, 0.80f, 0.070f, 1.0f, 1.0f, 0.90f, 0.15f);
        drawSmoothCircle(0.70f, 0.80f, 0.050f, 0.95f, 0.95f, 0.85f);
        drawSmoothCircle(0.68f, 0.81f, 0.048f, 0.02f, 0.03f, 0.07f);
    } else if (isRaining) {
        drawSolidBox(-1.0f, -0.25f, 1.0f, 1.0f, 0.22f, 0.25f, 0.30f);
    } else {
        drawSolidBox(-1.0f, -0.25f, 1.0f, 1.0f, 0.40f, 0.68f, 0.92f);
        drawSmoothCircle(0.75f, 0.80f, 0.08f, 1.0f, 0.90f, 0.30f, 0.25f);
        drawSmoothCircle(0.75f, 0.80f, 0.06f, 1.0f, 0.85f, 0.10f);
    }
}

// ID: SCENE3_RAIN_FOG
void renderRainAndFog3() {
    if (isFoggy) {
        float alpha = isNight ? 0.38f : 0.48f;
        drawSolidBox(-1.0f, -1.0f, 1.0f, 1.0f, 0.75f, 0.78f, 0.82f, alpha);
    }
    if (isRaining) {
        glColor4f(0.80f, 0.90f, 1.0f, 0.75f);
        glLineWidth(1.4f);
        glBegin(GL_LINES);
        for (int i = 0; i < MAX_RAIN; i++) {
            glVertex2f(rainX[i], rainY[i]);
            glVertex2f(rainX[i] - 0.012f, rainY[i] - 0.045f);
        }
        glEnd();
        glLineWidth(1.0f);
    }
}

// ID: SCENE3_LILY_PETAL
void drawWaterLilyPetal(float cx, float cy, float radius, float angle, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-radius * 0.30f, radius * 0.45f);
        glVertex2f(0.0f, radius);
        glVertex2f(radius * 0.30f, radius * 0.45f);
    glEnd();
    glPopMatrix();
}

// ID: SCENE3_LILY_FLOWER
void drawWaterLilyFlower(float cx, float cy, float scale) {
    for (int i = 0; i < 8; i++) drawWaterLilyPetal(cx, cy, 0.07f * scale, i * 45.0f, 0.94f, 0.45f, 0.65f);
    for (int i = 0; i < 8; i++) drawWaterLilyPetal(cx, cy, 0.05f * scale, i * 45.0f + 22.5f, 0.98f, 0.75f, 0.88f);
    drawSmoothCircle(cx, cy, 0.015f * scale, 1.0f, 0.85f, 0.10f);
}

// ID: SCENE3_TERM_EXTERIOR
void renderRealTerminal3Exterior3() {
    float wallR = isNight ? 0.08f : 0.20f;
    float wallG = isNight ? 0.12f : 0.25f;
    float wallB = isNight ? 0.16f : 0.30f;

    drawSolidBox(-0.55f, -0.10f, 0.95f, 0.48f, wallR, wallG, wallB);

    for (float x = -0.54f; x < 0.94f; x += 0.06f) {
        for (float y = -0.08f; y < 0.46f; y += 0.07f) {
            glColor3f(wallR + 0.12f, wallG + 0.12f, wallB + 0.14f);
            glBegin(GL_TRIANGLES);
                glVertex2f(x, y); glVertex2f(x + 0.06f, y); glVertex2f(x + 0.03f, y + 0.07f);
            glEnd();
            glColor3f(wallR - 0.04f, wallG - 0.04f, wallB - 0.03f);
            glBegin(GL_TRIANGLES);
                glVertex2f(x, y + 0.07f); glVertex2f(x + 0.06f, y + 0.07f); glVertex2f(x + 0.03f, y);
            glEnd();
            drawLine(x, y, x + 0.06f, y + 0.07f, wallR + 0.22f, wallG + 0.22f, wallB + 0.24f, 0.8f);
        }
    }

    drawWaterLilyFlower(-0.25f, 0.18f, 1.4f);
    drawWaterLilyFlower(0.20f, 0.28f, 1.6f);
    drawWaterLilyFlower(0.65f, 0.18f, 1.3f);

    drawSolidBox(-0.58f, 0.48f, 0.98f, 0.54f, 0.90f, 0.92f, 0.95f);
    drawSolidBox(-0.58f, 0.54f, 0.98f, 0.56f, 0.10f, 0.50f, 0.55f);

    drawBitmapText(-0.48f, 0.64f, "HAZRAT SHAHJALAL INTERNATIONAL AIRPORT", GLUT_BITMAP_HELVETICA_18, 0.05f, 0.20f, 0.45f);
    drawBitmapText(-0.10f, 0.58f, "TERMINAL 3", GLUT_BITMAP_HELVETICA_18, 0.90f, 0.15f, 0.15f);
}

// ID: SCENE3_FIRE_TRUCK
void renderAirportFireTruck3(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.85f, 0.85f, 1.0f);
    drawSolidBox(-0.16f, -0.02f, 0.16f, 0.07f, 0.85f, 0.10f, 0.10f);
    drawSolidBox(0.06f, 0.02f, 0.14f, 0.065f, 0.15f, 0.25f, 0.35f);
    drawSolidBox(-0.16f, 0.015f, 0.16f, 0.025f, 0.95f, 0.85f, 0.10f);
    drawLine(0.02f, 0.07f, 0.10f, 0.09f, 0.6f, 0.6f, 0.6f, 3.0f);

    if (emergencySiren3) drawSmoothCircle(0.02f, 0.08f, 0.014f, 1.0f, 0.0f, 0.0f);
    else drawSmoothCircle(-0.02f, 0.08f, 0.014f, 0.0f, 0.4f, 1.0f);

    drawSmoothCircle(-0.10f, -0.025f, 0.035f, 0.15f, 0.15f, 0.15f);
    drawSmoothCircle(-0.10f, -0.025f, 0.018f, 0.60f, 0.60f, 0.60f);
    drawSmoothCircle(0.00f, -0.025f, 0.035f, 0.15f, 0.15f, 0.15f);
    drawSmoothCircle(0.00f, -0.025f, 0.018f, 0.60f, 0.60f, 0.60f);
    drawSmoothCircle(0.10f, -0.025f, 0.035f, 0.15f, 0.15f, 0.15f);
    drawSmoothCircle(0.10f, -0.025f, 0.018f, 0.60f, 0.60f, 0.60f);
    glPopMatrix();
}

// ID: SCENE3_FUEL_DEPOT
void renderFuelDepotAndSecurity3() {
    drawSolidBox(-0.95f, -0.10f, -0.83f, 0.12f, 0.80f, 0.82f, 0.85f);
    drawSmoothCircle(-0.89f, 0.12f, 0.06f, 0.75f, 0.78f, 0.80f);
    drawBitmapText(-0.93f, 0.00f, "FUEL", GLUT_BITMAP_HELVETICA_10, 0.8f, 0.1f, 0.1f);
    drawSolidBox(-0.98f, -0.10f, -0.96f, 0.25f, 0.3f, 0.3f, 0.3f);
    drawSolidBox(-0.99f, 0.25f, -0.95f, 0.32f, 0.2f, 0.5f, 0.7f);
}

// ID: SCENE3_RUNWAY_LIGHTS
void renderRunwayLights3() {
    drawSolidBox(-1.0f, -0.18f, 1.0f, -0.10f, 0.22f, 0.22f, 0.24f);
    for (float x = -0.95f; x <= 0.95f; x += 0.12f) {
        if (isNight) {
            drawSmoothCircle(x, -0.10f, 0.008f, 0.1f, 1.0f, 0.2f);
            drawSmoothCircle(x, -0.18f, 0.008f, 1.0f, 0.2f, 0.1f);
        } else {
            drawSmoothCircle(x, -0.10f, 0.005f, 0.8f, 0.8f, 0.2f);
        }
    }
}

// ID: SCENE3_RADAR_TOWER
void renderRadarControlTower3() {
    float wallR = isNight ? 0.10f : 0.38f;
    float wallG = isNight ? 0.14f : 0.42f;
    float wallB = isNight ? 0.18f : 0.46f;

    glBegin(GL_POLYGON);
        glColor3f(wallR, wallG, wallB);
        glVertex2f(-0.75f, -0.10f); glVertex2f(-0.65f, -0.10f);
        glVertex2f(-0.67f, 0.58f); glVertex2f(-0.73f, 0.58f);
    glEnd();

    drawSmoothCircle(-0.70f, 0.63f, 0.075f, 0.15f, 0.55f, 0.75f, 0.85f);
    drawSolidBox(-0.77f, 0.58f, -0.63f, 0.60f, 0.2f, 0.2f, 0.22f);

    glPushMatrix();
    glTranslatef(-0.70f, 0.72f, 0.0f);
    glRotatef(radarAngle3, 0.0f, 0.0f, 1.0f);
    drawLine(-0.045f, 0.0f, 0.045f, 0.0f, 0.95f, 0.15f, 0.15f, 3.0f);
    drawSmoothCircle(0.045f, 0.0f, 0.01f, 0.2f, 0.2f, 0.2f);
    glPopMatrix();

    if (beaconState3) drawSmoothCircle(-0.70f, 0.74f, 0.014f, 1.0f, 0.1f, 0.1f);
}

// ID: SCENE3_ROAD_VEHICLES
void renderRoadwayAndVehicles3() {
    drawSolidBox(-1.0f, -0.35f, 1.0f, -0.22f, 0.55f, 0.55f, 0.58f);
    drawSolidBox(-1.0f, -1.00f, 1.0f, -0.35f, 0.15f, 0.15f, 0.17f);
    for (float x = -0.90f; x < 1.0f; x += 0.28f) drawSolidBox(x, -0.66f, x + 0.14f, -0.64f, 0.95f, 0.80f, 0.20f);

    float lamps[] = { -0.85f, -0.40f, 0.10f, 0.60f };
    for (int i = 0; i < 4; i++) {
        float lx = lamps[i];
        drawLine(lx, -0.22f, lx, 0.05f, 0.2f, 0.2f, 0.25f, 3.0f);
        drawLine(lx, 0.05f, lx + 0.04f, 0.05f, 0.2f, 0.2f, 0.25f, 2.0f);
        if (isNight) {
            drawSmoothCircle(lx + 0.04f, 0.05f, 0.025f, 1.0f, 0.9f, 0.4f);
            drawSolidBox(lx + 0.02f, -0.25f, lx + 0.08f, 0.04f, 1.0f, 0.9f, 0.5f, 0.08f);
        }
    }
}

// ID: SCENE3_SPORTS_CAR
void renderExpensiveSportsCar3(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.95f, 0.95f, 1.0f);
    drawSmoothCircle(-0.19f, -0.01f, 0.015f, 0.7f, 0.7f, 0.7f, 0.3f);
    drawSmoothCircle(-0.22f, 0.00f, 0.022f, 0.7f, 0.7f, 0.7f, 0.2f);
    glColor3f(0.95f, 0.20f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.16f, -0.02f); glVertex2f(0.16f, -0.02f); glVertex2f(0.18f, 0.005f);
        glVertex2f(0.08f, 0.025f); glVertex2f(-0.12f, 0.025f); glVertex2f(-0.17f, 0.00f);
    glEnd();
    glColor3f(0.10f, 0.12f, 0.15f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.06f, 0.025f); glVertex2f(0.06f, 0.025f); glVertex2f(0.02f, 0.065f); glVertex2f(-0.04f, 0.065f);
    glEnd();
    drawLine(-0.18f, 0.045f, -0.13f, 0.045f, 0.1f, 0.1f, 0.1f, 3.0f);
    drawSmoothCircle(-0.09f, -0.025f, 0.032f, 0.1f, 0.1f, 0.1f);
    drawSmoothCircle(-0.09f, -0.025f, 0.018f, 0.8f, 0.8f, 0.85f);
    drawSmoothCircle(0.09f, -0.025f, 0.032f, 0.1f, 0.1f, 0.1f);
    drawSmoothCircle(0.09f, -0.025f, 0.018f, 0.8f, 0.8f, 0.85f);
    if (isNight) drawSmoothCircle(0.17f, 0.005f, 0.012f, 0.9f, 0.95f, 1.0f);
    glPopMatrix();
}

// ID: SCENE3_STD_CAR
void renderStandardCar3(float x, float y, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    drawSolidBox(-0.13f, -0.03f, 0.13f, 0.03f, r, g, b);
    drawSolidBox(-0.07f, 0.03f, 0.05f, 0.075f, r * 0.85f, g * 0.85f, b * 0.85f);
    drawSmoothCircle(-0.07f, -0.03f, 0.03f, 0.1f, 0.1f, 0.1f);
    drawSmoothCircle(0.07f, -0.03f, 0.03f, 0.1f, 0.1f, 0.1f);
    if (isNight) drawSmoothCircle(0.13f, 0.00f, 0.012f, 1.0f, 1.0f, 0.5f);
    glPopMatrix();
}

// ID: SCENE3_HUD_DASH
void renderHUDDashboard3() {
    drawSolidBox(-0.98f, 0.85f, -0.45f, 0.96f, 0.0f, 0.0f, 0.0f, 0.5f);
    char modeStr[64];
    const char* timeMode = isNight ? "NIGHT" : "DAY";
    const char* weatherMode = isRaining ? "RAINING" : (isFoggy ? "FOGGY" : "CLEAR");
    sprintf(modeStr, "STATUS: %s | WEATHER: %s", timeMode, weatherMode);
    drawBitmapText(-0.96f, 0.89f, modeStr, GLUT_BITMAP_HELVETICA_10, 0.2f, 1.0f, 0.4f);
}

// ID: SCENE3_RENDER_FUNC
void drawScene3() {
    renderEnvironmentSky3();
    renderFuelDepotAndSecurity3();
    renderRadarControlTower3();
    renderRealTerminal3Exterior3();
    renderRunwayLights3();

    renderAirportFireTruck3(fireTruckX3, -0.14f);
    drawAirplane(planePosX3, 0.78f);

    renderRoadwayAndVehicles3();
    renderExpensiveSportsCar3(sportsCarX3, -0.82f);
    renderStandardCar3(luxurySUV_X3, -0.50f, 0.15f, 0.55f, 0.65f);
    renderStandardCar3(cargoVanX3, -0.52f, 0.85f, 0.85f, 0.85f);

    renderRainAndFog3();
    renderHUDDashboard3();

    drawText(-0.95f, 0.90f, 1.0f, 1.0f, 1.0f, "SCENE 3 (Press '1' for Scene 1, '2' for Scene 2)");
}

// ==========================================
// DISPLAY & MAIN CALLBACKS
// ==========================================
// ID: CALLBACK_DISPLAY
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (currentScene == 1) {
        drawScene1();
    } else if (currentScene == 2) {
        drawScene2();
    } else {
        drawScene3();
    }
    glFlush();
}

// ID: CALLBACK_TIMER
void timer(int value) {
    if (!isPaused) {
        car1X += 0.008f; car2X += 0.008f; car3X += 0.010f;
        if (car1X > 1.5f) car1X = -1.5f;
        if (car2X > 1.5f) car2X = -1.5f;
        if (car3X > 1.5f) car3X = -1.5f;

        bus1X += 0.0065f; bus2X += 0.0075f;
        if (bus1X > 1.5f) bus1X = -1.5f;
        if (bus2X > 1.5f) bus2X = -1.5f;

        wheelAngle -= 8.0f;
        if (wheelAngle <= -360.0f) wheelAngle = 0.0f;

        planeX1 += 0.0035f;
        if (planeX1 > 1.4f) planeX1 = -1.4f;

        planeParam2 += 0.004f;
        if (planeParam2 > 1.0f) planeParam2 = 0.0f;

        planePosX3 += 0.014f;
        if (planePosX3 > 1.40f) planePosX3 = -1.40f;

        fireTruckX3 += 0.010f;
        if (fireTruckX3 > 1.40f) fireTruckX3 = -1.40f;

        sportsCarX3 += 0.018f;
        if (sportsCarX3 > 1.45f) sportsCarX3 = -1.45f;

        luxurySUV_X3 += 0.007f;
        if (luxurySUV_X3 > 1.40f) luxurySUV_X3 = -1.40f;

        cargoVanX3 += 0.009f;
        if (cargoVanX3 > 1.40f) cargoVanX3 = -1.40f;

        radarAngle3 -= 4.0f;
        if (radarAngle3 < -360.0f) radarAngle3 = 0.0f;

        cloud1X += 0.0010f; cloud2X += 0.0006f; cloud3X += 0.0014f;
        if (cloud1X > 1.4f) cloud1X = -1.4f;
        if (cloud2X > 1.4f) cloud2X = -1.4f;
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

        sirenTicks3++;
        if (sirenTicks3 >= 10) {
            emergencySiren3 = !emergencySiren3;
            sirenTicks3 = 0;
        }

        beaconTicks3++;
        if (beaconTicks3 >= 20) {
            beaconState3 = !beaconState3;
            beaconTicks3 = 0;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ID: CALLBACK_KEYBOARD
void keyboard(unsigned char key, int x, int y) {
    if (key == '1') {
        currentScene = 1;
    } else if (key == '2') {
        currentScene = 2;
    } else if (key == '3') {
        currentScene = 3;
    } else if (key == 'n' || key == 'N') {
        isNight = true;
    } else if (key == 'd' || key == 'D') {
        isNight = false;
    } else if (key == 'r' || key == 'R') {
        isRaining = !isRaining;
    } else if (key == 'f' || key == 'F') {
        isFoggy = !isFoggy;
    } else if (key == 'p' || key == 'P' || key == ' ') {
        isPaused = !isPaused;
    }
    glutPostRedisplay();
}

// ID: CALLBACK_RESHAPE
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ID: MAIN_FUNC
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Complete Multi-Scene Airport & Terminal 3 Project");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initRain();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
