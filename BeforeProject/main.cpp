// =================================================================
// HAZRAT SHAHJALAL INTERNATIONAL AIRPORT
// AIR TRAFFIC MANAGEMENT CENTER + TERMINAL SCENE
//
// Controls:
//   N = Night
//   D = Day
//   P / SPACE = Pause
// =================================================================

#include <GL/glut.h>
#include <cmath>

// =================================================================
// GLOBALS
// =================================================================

float carX1 = -1.30f;
float carX2 =  0.30f;
float carX3 = -0.60f;
float busX  =  0.80f;

float planeX = -1.30f;
float wheelAngle = 0.0f;

bool isNight = false;
bool isPaused = false;

bool beaconOn = true;
int beaconCounter = 0;

const float PI = 3.14159265f;

// =================================================================
// BASIC SHAPES
// =================================================================

void drawRect(float x1, float y1,
              float x2, float y2,
              float r, float g, float b)
{
    glColor3f(r, g, b);

    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

// -----------------------------------------------------------------

void drawCircle(float cx, float cy, float radius,
                float r, float g, float b)
{
    glColor3f(r, g, b);

    glBegin(GL_POLYGON);

    for (int i = 0; i < 360; i += 5)
    {
        float a = i * PI / 180.0f;

        glVertex2f(
            cx + cos(a) * radius,
            cy + sin(a) * radius
        );
    }

    glEnd();
}

// -----------------------------------------------------------------

void drawLine(float x1, float y1,
              float x2, float y2,
              float r, float g, float b,
              float width = 1.0f)
{
    glColor3f(r, g, b);
    glLineWidth(width);

    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();

    glLineWidth(1.0f);
}

// -----------------------------------------------------------------

void drawText(float x, float y,
              const char* text,
              void* font,
              float r, float g, float b)
{
    glColor3f(r, g, b);
    glRasterPos2f(x, y);

    for (const char* c = text; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

// =================================================================
// SKY
// =================================================================

void drawSky()
{
    if (isNight)
    {
        drawRect(
            -1.0f, -0.20f,
             1.0f,  1.0f,
             0.035f, 0.055f, 0.13f
        );

        drawCircle(
            0.78f, 0.78f, 0.06f,
            0.95f, 0.95f, 0.85f
        );
    }
    else
    {
        drawRect(
            -1.0f, -0.20f,
             1.0f,  1.0f,
             0.47f, 0.75f, 0.90f
        );

        drawCircle(
            0.82f, 0.82f, 0.065f,
            1.0f, 0.85f, 0.25f
        );
    }
}

// =================================================================
// AIRPLANE
// =================================================================

void drawAirplane(float x, float y)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(0.85f, 0.85f, 1.0f);

    // Wings
    glColor3f(0.72f, 0.74f, 0.78f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.04f, 0.015f);
        glVertex2f(0.055f, 0.015f);
        glVertex2f(0.00f, 0.105f);
        glVertex2f(-0.05f, 0.105f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(-0.04f, -0.015f);
        glVertex2f(0.055f, -0.015f);
        glVertex2f(0.00f, -0.105f);
        glVertex2f(-0.05f, -0.105f);
    glEnd();

    // Tail wings
    glColor3f(0.65f, 0.68f, 0.72f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.14f, 0.012f);
        glVertex2f(-0.08f, 0.012f);
        glVertex2f(-0.12f, 0.05f);
        glVertex2f(-0.15f, 0.05f);
    glEnd();

    // Body
    drawRect(
        -0.14f, -0.019f,
         0.14f,  0.019f,
         0.94f, 0.94f, 0.92f
    );

    // Nose
    glColor3f(0.92f, 0.93f, 0.95f);

    glBegin(GL_POLYGON);
        glVertex2f(0.13f, -0.019f);
        glVertex2f(0.19f, -0.005f);
        glVertex2f(0.19f,  0.005f);
        glVertex2f(0.13f,  0.019f);
    glEnd();

    // Cockpit
    glColor3f(0.20f, 0.40f, 0.58f);

    glBegin(GL_POLYGON);
        glVertex2f(0.115f, 0.004f);
        glVertex2f(0.155f, 0.004f);
        glVertex2f(0.143f, 0.016f);
        glVertex2f(0.115f, 0.016f);
    glEnd();

    // Windows
    for (float px = -0.08f; px < 0.09f; px += 0.025f)
    {
        drawCircle(
            px, 0.007f, 0.004f,
            0.20f, 0.40f, 0.60f
        );
    }

    // Tail fin
    glColor3f(0.78f, 0.80f, 0.83f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.14f, 0.018f);
        glVertex2f(-0.08f, 0.018f);
        glVertex2f(-0.125f, 0.07f);
        glVertex2f(-0.155f, 0.07f);
    glEnd();

    glPopMatrix();
}

// =================================================================
// ATMCCT MAIN BUILDING
// =================================================================

void drawATMCCTBuilding()
{
    float wallR = isNight ? 0.28f : 0.70f;
    float wallG = isNight ? 0.29f : 0.69f;
    float wallB = isNight ? 0.31f : 0.67f;

    // Main office block
    drawRect(
        -0.92f, -0.10f,
        -0.38f,  0.52f,
        wallR, wallG, wallB
    );

    // Windows
    float glassR = isNight ? 0.12f : 0.45f;
    float glassG = isNight ? 0.25f : 0.65f;
    float glassB = isNight ? 0.38f : 0.78f;

    for (float x = -0.87f; x <= -0.45f; x += 0.085f)
    {
        drawRect(
            x, -0.04f,
            x + 0.055f, 0.46f,
            glassR, glassG, glassB
        );

        drawLine(
            x + 0.027f, -0.04f,
            x + 0.027f, 0.46f,
            0.25f, 0.27f, 0.28f, 1.0f
        );
    }

    for (float y = 0.08f; y < 0.46f; y += 0.10f)
    {
        drawLine(
            -0.91f, y,
            -0.40f, y,
            0.28f, 0.29f, 0.30f, 1.0f
        );
    }

    // Central block
    drawRect(
        -0.48f, -0.12f,
        -0.05f,  0.43f,
        wallR + 0.05f,
        wallG + 0.05f,
        wallB + 0.05f
    );

    // Circular window
    drawCircle(
        -0.265f, 0.17f, 0.145f,
        0.18f, 0.20f, 0.21f
    );

    drawCircle(
        -0.265f, 0.17f, 0.125f,
        isNight ? 0.12f : 0.30f,
        isNight ? 0.30f : 0.56f,
        isNight ? 0.42f : 0.67f
    );

    for (float y = 0.10f; y <= 0.24f; y += 0.045f)
    {
        float dy = y - 0.17f;
        float half = sqrt(0.125f * 0.125f - dy * dy);

        drawLine(
            -0.265f - half, y,
            -0.265f + half, y,
            0.15f, 0.18f, 0.20f, 1.2f
        );
    }

    for (float x = -0.31f; x <= -0.22f; x += 0.045f)
    {
        float dx = x + 0.265f;
        float half = sqrt(0.125f * 0.125f - dx * dx);

        drawLine(
            x, 0.17f - half,
            x, 0.17f + half,
            0.15f, 0.18f, 0.20f, 1.2f
        );
    }
}

// =================================================================
// CONTROL TOWER
// =================================================================

void drawControlTower()
{
    glColor3f(
        isNight ? 0.12f : 0.32f,
        isNight ? 0.30f : 0.56f,
        isNight ? 0.42f : 0.67f
    );

    glBegin(GL_POLYGON);
        glVertex2f(-0.39f, 0.38f);
        glVertex2f(-0.08f, 0.38f);
        glVertex2f(-0.105f, 0.58f);
        glVertex2f(-0.365f, 0.58f);
    glEnd();

    glColor3f(
        isNight ? 0.14f : 0.38f,
        isNight ? 0.34f : 0.63f,
        isNight ? 0.47f : 0.72f
    );

    glBegin(GL_POLYGON);
        glVertex2f(-0.365f, 0.58f);
        glVertex2f(-0.105f, 0.58f);
        glVertex2f(-0.08f, 0.73f);
        glVertex2f(-0.39f, 0.73f);
    glEnd();

    glColor3f(
        isNight ? 0.18f : 0.43f,
        isNight ? 0.39f : 0.68f,
        isNight ? 0.52f : 0.77f
    );

    glBegin(GL_POLYGON);
        glVertex2f(-0.39f, 0.73f);
        glVertex2f(-0.08f, 0.73f);
        glVertex2f(-0.05f, 0.83f);
        glVertex2f(-0.42f, 0.83f);
    glEnd();

    for (float y = 0.48f; y <= 0.78f; y += 0.09f)
    {
        drawLine(
            -0.38f, y,
            -0.09f, y,
            0.15f, 0.20f, 0.22f, 1.2f
        );
    }

    for (float x = -0.30f; x <= -0.17f; x += 0.065f)
    {
        drawLine(
            x, 0.45f,
            x, 0.80f,
            0.15f, 0.20f, 0.22f, 1.0f
        );
    }

    drawRect(
        -0.44f, 0.82f,
        -0.03f, 0.84f,
        0.20f, 0.22f, 0.23f
    );

    drawLine(
        -0.37f, 0.84f,
        -0.37f, 0.89f,
        0.18f, 0.18f, 0.20f, 1.0f
    );

    drawLine(
        -0.15f, 0.84f,
        -0.15f, 0.89f,
        0.18f, 0.18f, 0.20f, 1.0f
    );

    if (beaconOn)
    {
        drawCircle(
            -0.26f, 0.875f, 0.012f,
            1.0f, 0.12f, 0.08f
        );
    }
}

// =================================================================
// TERMINAL-1
// =================================================================

void drawTerminal()
{
    // Lower and wider than the tower.
    // Slightly separated from the ATMCCT structure.

    drawRect(
         0.03f, -0.12f,
         1.0f,  0.39f,
         isNight ? 0.40f : 0.80f,
         isNight ? 0.40f : 0.79f,
         isNight ? 0.38f : 0.72f
    );

    // Roof overhang
    drawRect(
        0.00f, 0.39f,
        1.00f, 0.45f,
        isNight ? 0.27f : 0.75f,
        isNight ? 0.27f : 0.73f,
        isNight ? 0.26f : 0.67f
    );

    // Terminal arches
    for (int i = 0; i < 5; i++)
    {
        float x = 0.09f + i * 0.19f;

        float glassR = isNight ? 0.12f : 0.40f;
        float glassG = isNight ? 0.27f : 0.62f;
        float glassB = isNight ? 0.40f : 0.73f;

        drawRect(
            x, -0.10f,
            x + 0.14f, 0.25f,
            glassR, glassG, glassB
        );

        // Arch
        glColor3f(glassR, glassG, glassB);

        glBegin(GL_POLYGON);

        for (int j = 180; j <= 360; j += 5)
        {
            float a = j * PI / 180.0f;

            glVertex2f(
                x + 0.07f + cos(a) * 0.07f,
                0.25f + sin(a) * 0.07f
            );
        }

        glVertex2f(x + 0.14f, -0.10f);
        glVertex2f(x, -0.10f);

        glEnd();

        // Center divider
        drawLine(
            x + 0.07f, -0.10f,
            x + 0.07f, 0.32f,
            0.16f, 0.20f, 0.22f, 1.2f
        );

        // Arch outline
        glColor3f(0.16f, 0.20f, 0.22f);
        glLineWidth(1.5f);

        glBegin(GL_LINE_STRIP);

        for (int j = 180; j <= 360; j += 5)
        {
            float a = j * PI / 180.0f;

            glVertex2f(
                x + 0.07f + cos(a) * 0.07f,
                0.25f + sin(a) * 0.07f
            );
        }

        glEnd();

        glLineWidth(1.0f);
    }

    // Signage
    drawText(
        0.17f, 0.47f,
        "SHAHJALAL INTERNATIONAL AIRPORT",
        GLUT_BITMAP_HELVETICA_18,
        0.75f, 0.08f, 0.06f
    );

    drawText(
        0.56f, 0.425f,
        "TERMINAL-1",
        GLUT_BITMAP_HELVETICA_12,
        0.05f, 0.55f, 0.20f
    );
}

// =================================================================
// PALM TREE
// =================================================================

void drawPalmTree(float x, float y)
{
    // Trunk
    glColor3f(0.45f, 0.28f, 0.13f);

    glBegin(GL_POLYGON);
        glVertex2f(x - 0.018f, y);
        glVertex2f(x + 0.014f, y);
        glVertex2f(x + 0.030f, y + 0.22f);
        glVertex2f(x - 0.002f, y + 0.22f);
    glEnd();

    // Trunk highlights
    drawLine(
        x - 0.005f, y + 0.02f,
        x + 0.018f, y + 0.20f,
        0.62f, 0.40f, 0.20f, 1.0f
    );

    // Crown
    float r = isNight ? 0.08f : 0.12f;
    float g = isNight ? 0.32f : 0.52f;
    float b = isNight ? 0.10f : 0.16f;

    // Center
    drawCircle(
        x + 0.015f, y + 0.23f,
        0.035f,
        0.18f, 0.38f, 0.10f
    );

    // Fronds
    glColor3f(r, g, b);

    glBegin(GL_POLYGON);
        glVertex2f(x + 0.02f, y + 0.24f);
        glVertex2f(x - 0.17f, y + 0.32f);
        glVertex2f(x - 0.08f, y + 0.25f);
        glVertex2f(x - 0.02f, y + 0.22f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(x + 0.02f, y + 0.25f);
        glVertex2f(x - 0.08f, y + 0.42f);
        glVertex2f(x - 0.01f, y + 0.31f);
        glVertex2f(x + 0.03f, y + 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(x + 0.025f, y + 0.25f);
        glVertex2f(x + 0.10f, y + 0.42f);
        glVertex2f(x + 0.08f, y + 0.29f);
        glVertex2f(x + 0.03f, y + 0.23f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(x + 0.025f, y + 0.24f);
        glVertex2f(x + 0.19f, y + 0.34f);
        glVertex2f(x + 0.10f, y + 0.23f);
        glVertex2f(x + 0.04f, y + 0.22f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(x + 0.015f, y + 0.23f);
        glVertex2f(x + 0.18f, y + 0.19f);
        glVertex2f(x + 0.08f, y + 0.21f);
        glVertex2f(x + 0.02f, y + 0.22f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(x + 0.01f, y + 0.24f);
        glVertex2f(x - 0.12f, y + 0.17f);
        glVertex2f(x - 0.04f, y + 0.22f);
        glVertex2f(x + 0.02f, y + 0.23f);
    glEnd();
}

// =================================================================
// ROAD
// =================================================================

void drawRoad()
{
    // Sidewalk
    drawRect(
        -1.0f, -0.40f,
         1.0f, -0.25f,
         0.70f, 0.68f, 0.62f
    );

    // Road
    drawRect(
        -1.0f, -1.0f,
         1.0f, -0.40f,
         0.28f, 0.28f, 0.29f
    );

    // Road boundaries
    drawLine(
        -1.0f, -0.40f,
         1.0f, -0.40f,
         0.12f, 0.12f, 0.12f, 2.0f
    );

    drawLine(
        -1.0f, -0.86f,
         1.0f, -0.86f,
         0.12f, 0.12f, 0.12f, 2.0f
    );

    // Lane markings
    for (float x = -0.95f; x < 1.0f; x += 0.27f)
    {
        drawRect(
            x, -0.66f,
            x + 0.13f, -0.645f,
            0.90f, 0.90f, 0.86f
        );
    }
}

// =================================================================
// LAMP POSTS
// =================================================================

void drawLampPost(float x)
{
    // Pole
    drawLine(
        x, -0.28f,
        x, -0.03f,
        0.12f, 0.12f, 0.13f, 3.0f
    );

    // Arm
    drawLine(
        x, -0.03f,
        x + 0.045f, -0.015f,
        0.12f, 0.12f, 0.13f, 2.5f
    );

    // Lamp
    if (isNight)
    {
        drawCircle(
            x + 0.047f, -0.015f,
            0.018f,
            1.0f, 0.85f, 0.45f
        );
    }
    else
    {
        drawCircle(
            x + 0.047f, -0.015f,
            0.014f,
            0.72f, 0.72f, 0.68f
        );
    }

    // Base
    drawRect(
        x - 0.012f, -0.285f,
        x + 0.012f, -0.275f,
        0.16f, 0.16f, 0.16f
    );
}

// =================================================================
// WHEEL
// =================================================================

void drawWheel(float x, float y)
{
    glPushMatrix();

    glTranslatef(x, y, 0);

    glRotatef(
        wheelAngle,
        0, 0, 1
    );

    drawCircle(
        0, 0, 0.045f,
        0.08f, 0.08f, 0.08f
    );

    drawCircle(
        0, 0, 0.022f,
        0.55f, 0.55f, 0.52f
    );

    drawLine(
        -0.035f, 0,
         0.035f, 0,
        0.18f, 0.18f, 0.18f, 1.0f
    );

    drawLine(
        0, -0.035f,
        0,  0.035f,
        0.18f, 0.18f, 0.18f, 1.0f
    );

    glPopMatrix();
}

// =================================================================
// CAR
// =================================================================

void drawCar(float x, float y, float scale = 1.0f)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);

    // Shadow
    drawCircle(
        0, -0.06f, 0.18f,
        0.20f, 0.20f, 0.19f
    );

    // Body
    drawRect(
        -0.18f, -0.045f,
         0.18f,  0.035f,
         0.82f, 0.83f, 0.80f
    );

    // Cabin
    glColor3f(0.76f, 0.77f, 0.75f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.11f, 0.035f);
        glVertex2f(0.09f, 0.035f);
        glVertex2f(0.045f, 0.115f);
        glVertex2f(-0.075f, 0.115f);
    glEnd();

    // Windows
    drawRect(
        -0.067f, 0.045f,
        -0.012f, 0.10f,
        0.28f, 0.48f, 0.58f
    );

    drawRect(
        0.005f, 0.045f,
        0.050f, 0.10f,
        0.28f, 0.48f, 0.58f
    );

    // Door
    drawLine(
        -0.01f, -0.04f,
        -0.01f, 0.035f,
        0.18f, 0.18f, 0.18f, 1.0f
    );

    // Lights
    drawCircle(
        0.18f, 0.005f, 0.012f,
        1.0f, 0.88f, 0.55f
    );

    drawCircle(
        -0.18f, 0.005f, 0.012f,
        0.70f, 0.12f, 0.10f
    );

    drawWheel(-0.105f, -0.045f);
    drawWheel( 0.105f, -0.045f);

    glPopMatrix();
}

// =================================================================
// BUS
// =================================================================

void drawBus(float x, float y)
{
    glPushMatrix();

    glTranslatef(x, y, 0);

    // Shadow
    drawCircle(
        0, -0.055f, 0.27f,
        0.18f, 0.18f, 0.18f
    );

    // Main body
    drawRect(
        -0.27f, -0.06f,
         0.27f,  0.11f,
         0.78f, 0.78f, 0.76f
    );

    // Lower section
    drawRect(
        -0.27f, -0.06f,
         0.27f,  0.00f,
         0.55f, 0.58f, 0.60f
    );

    // Windows
    for (float wx = -0.21f; wx <= 0.14f; wx += 0.09f)
    {
        drawRect(
            wx, 0.025f,
            wx + 0.065f, 0.095f,
            0.18f, 0.40f, 0.52f
        );
    }

    // Front windshield
    drawRect(
        0.18f, 0.025f,
        0.24f, 0.095f,
        0.15f, 0.35f, 0.48f
    );

    // Wheels
    drawCircle(
        -0.17f, -0.06f, 0.055f,
        0.06f, 0.06f, 0.06f
    );

    drawCircle(
        0.17f, -0.06f, 0.055f,
        0.06f, 0.06f, 0.06f
    );

    // Bus stripe
    drawRect(
        -0.27f, 0.00f,
         0.27f, 0.018f,
         0.06f, 0.48f, 0.24f
    );

    glPopMatrix();
}

// =================================================================
// DISPLAY
// =================================================================

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();

    // Airplane
    drawAirplane(planeX, 0.78f);

    // Main buildings
    drawATMCCTBuilding();
    drawControlTower();
    drawTerminal();

    // Palm
    drawPalmTree(-0.08f, -0.10f);

    // Road
    drawRoad();

    // Lamp posts
    drawLampPost(-0.82f);
    drawLampPost(-0.48f);
    drawLampPost(-0.14f);
    drawLampPost( 0.35f);
    drawLampPost( 0.78f);

    // Cars
    drawCar(carX1, -0.52f, 1.0f);
    drawCar(carX2, -0.72f, 0.82f);
    drawCar(carX3, -0.89f, 0.90f);

    // Bus
    drawBus(busX, -0.64f);

    glFlush();
}

// =================================================================
// ANIMATION
// =================================================================

void timer(int value)
{
    if (!isPaused)
    {
        // ---------------------------------------------------------
        // Cars
        // ---------------------------------------------------------

        carX1 += 0.007f;

        if (carX1 > 1.35f)
            carX1 = -1.35f;

        carX2 += 0.005f;

        if (carX2 > 1.35f)
            carX2 = -1.35f;

        carX3 += 0.009f;

        if (carX3 > 1.35f)
            carX3 = -1.35f;

        // ---------------------------------------------------------
        // Bus
        // ---------------------------------------------------------

        busX += 0.004f;

        if (busX > 1.45f)
            busX = -1.45f;

        // ---------------------------------------------------------
        // Wheels
        // ---------------------------------------------------------

        wheelAngle -= 10.0f;

        if (wheelAngle < -360.0f)
            wheelAngle = 0.0f;

        // ---------------------------------------------------------
        // Plane
        // Faster than cars
        // ---------------------------------------------------------

        planeX += 0.014f;

        if (planeX > 1.35f)
            planeX = -1.35f;

        // ---------------------------------------------------------
        // Beacon
        // ---------------------------------------------------------

        beaconCounter++;

        if (beaconCounter >= 30)
        {
            beaconOn = !beaconOn;
            beaconCounter = 0;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// =================================================================
// KEYBOARD
// =================================================================

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'n' || key == 'N')
        isNight = true;

    else if (key == 'd' || key == 'D')
        isNight = false;

    else if (key == 'p' || key == 'P' || key == ' ')
        isPaused = !isPaused;

    glutPostRedisplay();
}

// =================================================================
// RESHAPE
// =================================================================

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(
        -1.0, 1.0,
        -1.0, 1.0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// =================================================================
// MAIN
// =================================================================

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_SINGLE | GLUT_RGB
    );

    glutInitWindowSize(1024, 700);
    glutInitWindowPosition(100, 50);

    glutCreateWindow(
        "Hazrat Shahjalal International Airport"
    );

    glClearColor(
        0.45f, 0.72f, 0.88f, 1.0f
    );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(
        -1.0, 1.0,
        -1.0, 1.0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();

    return 0;
}
