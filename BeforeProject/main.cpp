#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

// =================================================================
// HAZRAT SHAHJALAL INTERNATIONAL AIRPORT
// ATMCCT + CONTROL TOWER + TERMINAL
// =================================================================

// -------------------------------------------------------------
// Global Variables
// -------------------------------------------------------------
float car1X = -1.30f;
float car2X = -0.65f;
float car3X = -0.95f;

float bus1X = 0.15f;
float bus2X = -0.35f;

float planeX = -1.40f;

float wheelAngle = 0.0f;

float cloud1X = -0.70f;
float cloud2X = 0.20f;
float cloud3X = -1.20f;

bool isNight = false;
bool isRaining = false;
bool isPaused = false;

const int MAX_RAIN = 120;

float rainX[MAX_RAIN];
float rainY[MAX_RAIN];
float rainSpeed[MAX_RAIN];


// -------------------------------------------------------------
// Rain Initialization
// -------------------------------------------------------------
void initRain()
{
    for (int i = 0; i < MAX_RAIN; i++)
    {
        rainX[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        rainY[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        rainSpeed[i] = 0.025f +
                       ((float)rand() / RAND_MAX) * 0.02f;
    }
}


// -------------------------------------------------------------
// Basic Rectangle
// -------------------------------------------------------------
void drawRect(float x1, float y1,
              float x2, float y2,
              float r, float g, float b)
{
    glColor3f(r, g, b);

    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}


// -------------------------------------------------------------
// Circle
// -------------------------------------------------------------
void drawCircle(float cx, float cy,
                float radius,
                float r, float g, float b)
{
    glColor3f(r, g, b);

    glBegin(GL_POLYGON);

    for (int i = 0; i < 360; i += 15)
    {
        float rad = i * 3.14159265f / 180.0f;

        glVertex2f(
            cx + cos(rad) * radius,
            cy + sin(rad) * radius
        );
    }

    glEnd();
}


// -------------------------------------------------------------
// Line
// -------------------------------------------------------------
void drawLine(float x1, float y1,
              float x2, float y2,
              float r, float g, float b,
              float width)
{
    glColor3f(r, g, b);
    glLineWidth(width);

    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
}


// =================================================================
// SKY + CLOUDS
// =================================================================
void drawCloud(float x, float y, float scale)
{
    float baseR, baseG, baseB;
    float highR, highG, highB;

    if (isRaining)
    {
        baseR = isNight ? 0.10f : 0.40f;
        baseG = isNight ? 0.12f : 0.43f;
        baseB = isNight ? 0.16f : 0.48f;

        highR = isNight ? 0.15f : 0.50f;
        highG = isNight ? 0.17f : 0.53f;
        highB = isNight ? 0.22f : 0.58f;
    }
    else
    {
        baseR = isNight ? 0.20f : 0.90f;
        baseG = isNight ? 0.24f : 0.92f;
        baseB = isNight ? 0.32f : 0.96f;

        highR = isNight ? 0.28f : 1.00f;
        highG = isNight ? 0.32f : 1.00f;
        highB = isNight ? 0.40f : 1.00f;
    }

    drawCircle(
        x - 0.06f * scale,
        y,
        0.045f * scale,
        baseR, baseG, baseB
    );

    drawCircle(
        x + 0.06f * scale,
        y,
        0.045f * scale,
        baseR, baseG, baseB
    );

    drawCircle(
        x,
        y + 0.025f * scale,
        0.060f * scale,
        highR, highG, highB
    );

    drawCircle(
        x - 0.035f * scale,
        y + 0.012f * scale,
        0.048f * scale,
        highR, highG, highB
    );

    drawCircle(
        x + 0.035f * scale,
        y + 0.015f * scale,
        0.048f * scale,
        highR, highG, highB
    );

    drawRect(
        x - 0.06f * scale,
        y - 0.035f * scale,
        x + 0.06f * scale,
        y + 0.005f * scale,
        baseR, baseG, baseB
    );
}


void drawSky()
{
    if (isRaining)
    {
        if (isNight)
        {
            drawRect(
                -1.0f, -0.28f,
                1.0f, 1.0f,
                0.03f, 0.04f, 0.10f
            );

            drawCircle(
                -0.80f, 0.80f, 0.075f,
                0.55f, 0.55f, 0.50f
            );
        }
        else
        {
            drawRect(
                -1.0f, -0.28f,
                1.0f, 1.0f,
                0.42f, 0.48f, 0.55f
            );

            drawCircle(
                -0.80f, 0.80f, 0.080f,
                0.80f, 0.75f, 0.50f
            );
        }
    }
    else
    {
        if (isNight)
        {
            drawRect(
                -1.0f, -0.28f,
                1.0f, 1.0f,
                0.06f, 0.08f, 0.18f
            );

            drawCircle(
                -0.80f, 0.80f, 0.080f,
                0.95f, 0.95f, 0.85f
            );
        }
        else
        {
            drawRect(
                -1.0f, -0.28f,
                1.0f, 1.0f,
                0.55f, 0.78f, 0.98f
            );

            drawCircle(
                -0.80f, 0.80f, 0.085f,
                1.0f, 0.85f, 0.20f
            );
        }
    }

    drawCloud(cloud1X, 0.62f, 1.25f);
    drawCloud(cloud2X, 0.44f, 1.05f);
    drawCloud(cloud3X, 0.53f, 0.90f);
}


// =================================================================
// AIRPLANE
// =================================================================
void drawAirplane(float x, float y)
{
    glPushMatrix();

    glTranslatef(x, y, 0.0f);

    // Main wings
    glColor3f(0.72f, 0.75f, 0.82f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.04f, 0.018f);
        glVertex2f(0.05f, 0.018f);
        glVertex2f(0.00f, 0.11f);
        glVertex2f(-0.05f, 0.11f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(-0.04f, -0.018f);
        glVertex2f(0.05f, -0.018f);
        glVertex2f(0.00f, -0.11f);
        glVertex2f(-0.05f, -0.11f);
    glEnd();

    // Tail wings
    glColor3f(0.68f, 0.70f, 0.76f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.13f, 0.012f);
        glVertex2f(-0.08f, 0.012f);
        glVertex2f(-0.11f, 0.045f);
        glVertex2f(-0.14f, 0.045f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(-0.13f, -0.012f);
        glVertex2f(-0.08f, -0.012f);
        glVertex2f(-0.11f, -0.045f);
        glVertex2f(-0.14f, -0.045f);
    glEnd();

    // Body
    drawRect(
        -0.14f, -0.020f,
        0.13f, 0.020f,
        0.94f, 0.95f, 0.98f
    );

    // Nose
    glColor3f(0.94f, 0.95f, 0.98f);

    glBegin(GL_POLYGON);
        glVertex2f(0.13f, -0.020f);
        glVertex2f(0.18f, -0.006f);
        glVertex2f(0.18f, 0.006f);
        glVertex2f(0.13f, 0.020f);
    glEnd();

    // Red stripe
    drawRect(
        -0.14f, -0.003f,
        0.14f, 0.003f,
        0.85f, 0.20f, 0.20f
    );

    // Cockpit
    glColor3f(0.20f, 0.40f, 0.65f);

    glBegin(GL_POLYGON);
        glVertex2f(0.12f, 0.004f);
        glVertex2f(0.155f, 0.004f);
        glVertex2f(0.145f, 0.016f);
        glVertex2f(0.12f, 0.016f);
    glEnd();

    // Windows
    for (float px = -0.08f; px <= 0.09f; px += 0.022f)
    {
        drawCircle(
            px, 0.008f,
            0.004f,
            0.25f, 0.45f, 0.70f
        );
    }

    // Tail
    glColor3f(0.85f, 0.20f, 0.20f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.14f, 0.020f);
        glVertex2f(-0.08f, 0.020f);
        glVertex2f(-0.13f, 0.075f);
        glVertex2f(-0.16f, 0.075f);
    glEnd();

    glPopMatrix();
}


// =================================================================
// ORIGINAL ATMCCT BUILDING
// =================================================================
void drawATMCCTBuilding()
{
    // Main left office block
    float bodyR = isNight ? 0.28f : 0.78f;
    float bodyG = isNight ? 0.30f : 0.80f;
    float bodyB = isNight ? 0.36f : 0.82f;

    drawRect(
        -0.90f, -0.28f,
        -0.47f, 0.25f,
        bodyR, bodyG, bodyB
    );

    // Lower shadow
    drawRect(
        -0.90f, -0.28f,
        -0.47f, -0.18f,
        0.25f, 0.27f, 0.30f
    );

    // Main glass façade
    float glassR = isNight ? 0.12f : 0.30f;
    float glassG = isNight ? 0.28f : 0.62f;
    float glassB = isNight ? 0.42f : 0.82f;

    drawRect(
        -0.86f, -0.12f,
        -0.51f, 0.19f,
        glassR, glassG, glassB
    );

    // Glass grid
    for (float x = -0.86f; x <= -0.51f; x += 0.058f)
    {
        drawLine(
            x, -0.12f,
            x, 0.19f,
            0.10f, 0.13f, 0.16f,
            1.5f
        );
    }

    for (float y = -0.12f; y <= 0.19f; y += 0.065f)
    {
        drawLine(
            -0.86f, y,
            -0.51f, y,
            0.10f, 0.13f, 0.16f,
            1.5f
        );
    }

    // Upper roof
    glColor3f(0.25f, 0.27f, 0.30f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.91f, 0.25f);
        glVertex2f(-0.46f, 0.25f);
        glVertex2f(-0.50f, 0.34f);
        glVertex2f(-0.88f, 0.34f);
    glEnd();

    // Orange architectural edge
    glColor3f(0.85f, 0.40f, 0.15f);

    glBegin(GL_POLYGON);
        glVertex2f(-0.88f, 0.34f);
        glVertex2f(-0.91f, 0.25f);
        glVertex2f(-0.88f, 0.25f);
        glVertex2f(-0.85f, 0.33f);
    glEnd();

    // Roof supports
    for (float x = -0.85f; x <= -0.55f; x += 0.10f)
    {
        drawLine(
            x, 0.25f,
            x - 0.035f, 0.32f,
            0.18f, 0.19f, 0.22f,
            2.5f
        );
    }

    // Small antenna
    drawLine(
        -0.69f, 0.34f,
        -0.69f, 0.48f,
        0.20f, 0.20f, 0.22f,
        2
    );

    drawLine(
        -0.73f, 0.42f,
        -0.65f, 0.42f,
        0.20f, 0.20f, 0.22f,
        1.5f
    );

    drawCircle(
        -0.69f, 0.49f,
        0.012f,
        1.0f, 0.10f, 0.10f
    );
}


// =================================================================
// CONTROL TOWER
// =================================================================
void drawControlTower()
{
    // Tower shaft
    glColor3f(
        isNight ? 0.28f : 0.63f,
        isNight ? 0.30f : 0.65f,
        isNight ? 0.35f : 0.68f
    );

    glBegin(GL_POLYGON);
        glVertex2f(-0.30f, -0.28f);
        glVertex2f(-0.05f, -0.28f);
        glVertex2f(-0.09f, 0.57f);
        glVertex2f(-0.25f, 0.57f);
    glEnd();

    // Glass tower
    drawRect(
        -0.23f, -0.05f,
        -0.11f, 0.50f,
        isNight ? 0.10f : 0.18f,
        isNight ? 0.25f : 0.45f,
        isNight ? 0.38f : 0.60f
    );

    // Vertical glass frames
    drawLine(
        -0.23f, -0.05f,
        -0.23f, 0.50f,
        0.10f, 0.12f, 0.15f,
        2
    );

    drawLine(
        -0.17f, -0.05f,
        -0.17f, 0.50f,
        0.10f, 0.12f, 0.15f,
        2
    );

    drawLine(
        -0.11f, -0.05f,
        -0.11f, 0.50f,
        0.10f, 0.12f, 0.15f,
        2
    );

    // Observation deck
    drawRect(
        -0.28f, 0.50f,
        -0.06f, 0.57f,
        0.22f, 0.24f, 0.27f
    );

    // Glass observation windows
    drawRect(
        -0.25f, 0.515f,
        -0.09f, 0.555f,
        isNight ? 0.18f : 0.25f,
        isNight ? 0.35f : 0.55f,
        isNight ? 0.50f : 0.72f
    );

    // Roof cap
    drawRect(
        -0.30f, 0.57f,
        -0.04f, 0.61f,
        0.30f, 0.32f, 0.35f
    );

    // Antennas
    drawLine(
        -0.17f, 0.61f,
        -0.17f, 0.78f,
        0.18f, 0.19f, 0.22f,
        2
    );

    drawLine(
        -0.21f, 0.70f,
        -0.13f, 0.70f,
        0.18f, 0.19f, 0.22f,
        1.5f
    );

    // Blinking beacon
    static int beaconCounter = 0;

    if (beaconCounter < 25)
    {
        drawCircle(
            -0.17f, 0.80f,
            0.014f,
            1.0f, 0.05f, 0.05f
        );
    }

    beaconCounter++;
    if (beaconCounter >= 60)
        beaconCounter = 0;
}


// =================================================================
// TERMINAL
// =================================================================
void drawTerminal()
{
    // Long low terminal body
    float r = isNight ? 0.30f : 0.82f;
    float g = isNight ? 0.33f : 0.80f;
    float b = isNight ? 0.39f : 0.76f;

    drawRect(
        0.02f, -0.28f,
        0.88f, 0.22f,
        r, g, b
    );

    // Lower dark section
    drawRect(
        0.02f, -0.28f,
        0.88f, -0.10f,
        0.25f, 0.27f, 0.30f
    );

    // Glass façade
    drawRect(
        0.07f, -0.08f,
        0.83f, 0.17f,
        isNight ? 0.10f : 0.28f,
        isNight ? 0.28f : 0.62f,
        isNight ? 0.42f : 0.82f
    );

    // Glass grid
    for (float x = 0.07f; x <= 0.83f; x += 0.075f)
    {
        drawLine(
            x, -0.08f,
            x, 0.17f,
            0.12f, 0.14f, 0.17f,
            1.5f
        );
    }

    for (float y = -0.08f; y <= 0.17f; y += 0.065f)
    {
        drawLine(
            0.07f, y,
            0.83f, y,
            0.12f, 0.14f, 0.17f,
            1.5f
        );
    }

    // Terminal roof
    drawRect(
        -0.02f, 0.22f,
        0.91f, 0.28f,
        0.24f, 0.26f, 0.29f
    );

    drawRect(
        0.02f, 0.28f,
        0.88f, 0.32f,
        0.62f, 0.64f, 0.67f
    );

    // Large entrance sections
    for (int i = 0; i < 5; i++)
    {
        float x = 0.13f + i * 0.15f;

        // Pillar
        drawRect(
            x - 0.018f, -0.28f,
            x + 0.018f, 0.13f,
            0.76f, 0.77f, 0.79f
        );

        // Entrance
        drawRect(
            x - 0.055f, -0.28f,
            x + 0.055f, 0.10f,
            isNight ? 0.10f : 0.25f,
            isNight ? 0.28f : 0.60f,
            isNight ? 0.40f : 0.78f
        );

        // Arch top
        drawCircle(
            x, 0.10f,
            0.055f,
            0.72f, 0.74f, 0.77f
        );

        drawRect(
            x - 0.055f, 0.05f,
            x + 0.055f, 0.11f,
            isNight ? 0.10f : 0.25f,
            isNight ? 0.28f : 0.60f,
            isNight ? 0.40f : 0.78f
        );
    }

    // Airport terminal sign
    drawRect(
        0.34f, 0.20f,
        0.63f, 0.26f,
        0.04f, 0.25f, 0.46f
    );

    drawRect(
        0.37f, 0.215f,
        0.60f, 0.245f,
        0.07f, 0.43f, 0.66f
    );
}


// =================================================================
// TREES
// =================================================================
void drawNaturalTree(float x, float y, float scale)
{
    // Trunk
    drawRect(
        x - 0.015f * scale,
        y,
        x + 0.015f * scale,
        y + 0.30f * scale,
        0.34f, 0.20f, 0.10f
    );

    // Branches
    drawLine(
        x, y + 0.18f * scale,
        x - 0.07f * scale,
        y + 0.30f * scale,
        0.34f, 0.20f, 0.10f,
        3
    );

    drawLine(
        x, y + 0.20f * scale,
        x + 0.07f * scale,
        y + 0.32f * scale,
        0.34f, 0.20f, 0.10f,
        3
    );

    float darkG  = isNight ? 0.24f : 0.43f;
    float midG   = isNight ? 0.34f : 0.58f;
    float lightG = isNight ? 0.43f : 0.70f;

    // Foliage
    drawCircle(
        x - 0.065f * scale,
        y + 0.31f * scale,
        0.065f * scale,
        0.07f, darkG, 0.12f
    );

    drawCircle(
        x + 0.065f * scale,
        y + 0.32f * scale,
        0.065f * scale,
        0.07f, darkG, 0.12f
    );

    drawCircle(
        x - 0.045f * scale,
        y + 0.40f * scale,
        0.075f * scale,
        0.08f, midG, 0.14f
    );

    drawCircle(
        x + 0.045f * scale,
        y + 0.41f * scale,
        0.075f * scale,
        0.08f, midG, 0.14f
    );

    drawCircle(
        x,
        y + 0.49f * scale,
        0.080f * scale,
        0.10f, lightG, 0.17f
    );
}


// =================================================================
// STREET LAMPS
// =================================================================
void drawStreetLamp(float x, float y, float height, bool faceRight)
{
    // Base
    drawRect(
        x - 0.014f, y,
        x + 0.014f, y + 0.035f,
        0.25f, 0.25f, 0.28f
    );

    float armDir = faceRight ? 0.12f : -0.12f;

    // Pole + arm
    drawLine(
        x, y,
        x, y + height,
        0.15f, 0.15f, 0.18f,
        4.5f
    );

    drawLine(
        x, y + height,
        x + armDir,
        y + height + 0.02f,
        0.15f, 0.15f, 0.18f,
        4.5f
    );

    drawLine(
        x + armDir,
        y + height + 0.02f,
        x + armDir,
        y + height - 0.025f,
        0.15f, 0.15f, 0.18f,
        4.5f
    );

    // Lamp housing
    glColor3f(0.10f, 0.12f, 0.15f);

    glBegin(GL_POLYGON);
        glVertex2f(
            x + armDir - 0.035f,
            y + height - 0.015f
        );

        glVertex2f(
            x + armDir + 0.035f,
            y + height - 0.015f
        );

        glVertex2f(
            x + armDir + 0.022f,
            y + height + 0.020f
        );

        glVertex2f(
            x + armDir - 0.022f,
            y + height + 0.020f
        );
    glEnd();

    // Light
    if (isNight || isRaining)
    {
        drawCircle(
            x + armDir,
            y + height - 0.038f,
            0.040f,
            1.0f, 0.90f, 0.35f
        );
    }

    drawCircle(
        x + armDir,
        y + height - 0.035f,
        0.020f,
        1.0f, 0.95f, 0.45f
    );
}


// =================================================================
// ROAD
// =================================================================
void drawRoad()
{
    // Sidewalk
    drawRect(
        -1.0f, -0.36f,
        1.0f, -0.28f,
        0.58f, 0.58f, 0.60f
    );

    // Sidewalk tiles
    glColor3f(0.30f, 0.30f, 0.30f);
    glLineWidth(1.5f);

    for (float x = -1.0f; x <= 1.0f; x += 0.10f)
    {
        glBegin(GL_LINES);
            glVertex2f(x, -0.36f);
            glVertex2f(x, -0.28f);
        glEnd();
    }

    // Road
    drawRect(
        -1.0f, -1.0f,
        1.0f, -0.36f,
        0.22f, 0.22f, 0.24f
    );

    // Yellow center line
    drawRect(
        -1.0f, -0.69f,
        1.0f, -0.675f,
        0.90f, 0.75f, 0.10f
    );

    // White lane markings
    for (float x = -0.95f; x < 1.0f; x += 0.25f)
    {
        drawRect(
            x, -0.54f,
            x + 0.12f, -0.52f,
            1.0f, 1.0f, 1.0f
        );

        drawRect(
            x, -0.86f,
            x + 0.12f, -0.84f,
            1.0f, 1.0f, 1.0f
        );
    }
}


// =================================================================
// WHEEL
// =================================================================
void drawRotatingWheel(
    float cx,
    float cy,
    float radius
)
{
    glPushMatrix();

    glTranslatef(cx, cy, 0.0f);
    glRotatef(
        wheelAngle,
        0.0f, 0.0f, 1.0f
    );

    drawCircle(
        0, 0,
        radius,
        0.10f, 0.10f, 0.11f
    );

    drawCircle(
        0, 0,
        radius * 0.45f,
        0.70f, 0.70f, 0.72f
    );

    for (int i = 0; i < 4; i++)
    {
        float a =
            i * 3.14159265f / 2.0f;

        drawLine(
            0, 0,
            cos(a) * radius * 0.70f,
            sin(a) * radius * 0.70f,
            0.18f, 0.18f, 0.20f,
            1.5f
        );
    }

    glPopMatrix();
}


// =================================================================
// CAR
// =================================================================
void drawCar(
    float x,
    float y,
    float r,
    float g,
    float b
)
{
    glPushMatrix();

    glTranslatef(x, y, 0.0f);

    // Body
    drawRect(
        -0.16f, -0.04f,
        0.16f, 0.045f,
        r, g, b
    );

    // Cabin
    glColor3f(
        r * 0.90f,
        g * 0.90f,
        b * 0.90f
    );

    glBegin(GL_POLYGON);
        glVertex2f(-0.10f, 0.045f);
        glVertex2f(0.08f, 0.045f);
        glVertex2f(0.05f, 0.11f);
        glVertex2f(-0.07f, 0.11f);
    glEnd();

    // Windows
    float winCol = isNight ? 0.35f : 0.75f;

    drawRect(
        -0.06f, 0.055f,
        -0.015f, 0.10f,
        winCol,
        winCol + 0.15f,
        0.95f
    );

    drawRect(
        0.005f, 0.055f,
        0.045f, 0.10f,
        winCol,
        winCol + 0.15f,
        0.95f
    );

    // Lights
    drawRect(
        0.15f, 0.00f,
        0.16f, 0.035f,
        1.0f, 0.95f, 0.20f
    );

    drawRect(
        -0.16f, 0.00f,
        -0.15f, 0.035f,
        0.90f, 0.10f, 0.10f
    );

    drawRotatingWheel(
        -0.09f, -0.04f,
        0.035f
    );

    drawRotatingWheel(
        0.09f, -0.04f,
        0.035f
    );

    glPopMatrix();
}


// =================================================================
// BUS
// =================================================================
void drawBus(
    float x,
    float y,
    float r,
    float g,
    float b,
    float stripeR,
    float stripeG,
    float stripeB
)
{
    glPushMatrix();

    glTranslatef(x, y, 0.0f);

    drawRect(
        -0.30f, -0.05f,
        0.30f, 0.17f,
        r, g, b
    );

    drawRect(
        -0.30f, 0.17f,
        0.30f, 0.19f,
        r * 0.85f,
        g * 0.85f,
        b * 0.85f
    );

    drawRect(
        -0.30f, 0.025f,
        0.30f, 0.065f,
        stripeR, stripeG, stripeB
    );

    float winCol = isNight ? 0.35f : 0.75f;

    // Windows
    for (int i = 0; i < 5; i++)
    {
        float wx = -0.29f + i * 0.095f;

        drawRect(
            wx, 0.065f,
            wx + 0.075f, 0.155f,
            winCol,
            winCol + 0.15f,
            0.95f
        );

        glColor3f(
            0.15f, 0.15f, 0.18f
        );

        glLineWidth(1.5f);

        glBegin(GL_LINE_LOOP);
            glVertex2f(wx, 0.065f);
            glVertex2f(wx + 0.075f, 0.065f);
            glVertex2f(wx + 0.075f, 0.155f);
            glVertex2f(wx, 0.155f);
        glEnd();
    }

    // Front window
    drawRect(
        0.20f, 0.065f,
        0.30f, 0.155f,
        winCol,
        winCol + 0.15f,
        0.95f
    );

    // Lights
    drawRect(
        0.30f, -0.02f,
        0.32f, 0.035f,
        1.0f, 0.95f, 0.20f
    );

    drawRect(
        -0.32f, -0.02f,
        -0.30f, 0.035f,
        0.90f, 0.10f, 0.10f
    );

    drawRotatingWheel(
        -0.20f, -0.05f,
        0.048f
    );

    drawRotatingWheel(
        0.20f, -0.05f,
        0.048f
    );

    glPopMatrix();
}


// =================================================================
// RAIN
// =================================================================
void drawRain()
{
    if (!isRaining)
        return;

    glColor3f(
        0.75f, 0.85f, 0.98f
    );

    glLineWidth(1.5f);

    glBegin(GL_LINES);

    for (int i = 0; i < MAX_RAIN; i++)
    {
        glVertex2f(
            rainX[i],
            rainY[i]
        );

        glVertex2f(
            rainX[i] - 0.012f,
            rainY[i] - 0.045f
        );
    }

    glEnd();
}


// =================================================================
// DISPLAY
// =================================================================
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Sky
    drawSky();

    // Plane
    drawAirplane(
        planeX,
        0.80f
    );

    // Background trees
    drawNaturalTree(
        -0.96f,
        -0.28f,
        1.05f
    );

    drawNaturalTree(
        0.92f,
        -0.28f,
        1.10f
    );

    drawNaturalTree(
        0.82f,
        -0.28f,
        0.80f
    );

    // Original airport structures
    drawATMCCTBuilding();

    drawControlTower();

    drawTerminal();

    // Street lamps
    drawStreetLamp(
        -0.43f,
        -0.28f,
        0.58f,
        false
    );

    drawStreetLamp(
        0.05f,
        -0.28f,
        0.54f,
        false
    );

    drawStreetLamp(
        0.63f,
        -0.28f,
        0.46f,
        false
    );

    // Road
    drawRoad();

    // Traffic
    drawCar(
        car1X,
        -0.48f,
        0.85f, 0.20f, 0.20f
    );

    drawCar(
        car2X,
        -0.48f,
        0.20f, 0.55f, 0.85f
    );

    drawBus(
        bus1X,
        -0.48f,
        0.10f, 0.60f, 0.65f,
        1.0f, 0.85f, 0.20f
    );

    drawCar(
        car3X,
        -0.82f,
        0.88f, 0.65f, 0.15f
    );

    drawBus(
        bus2X,
        -0.82f,
        0.90f, 0.40f, 0.15f,
        0.95f, 0.95f, 0.95f
    );

    // Rain foreground
    drawRain();

    glFlush();
}


// =================================================================
// TIMER
// =================================================================
void timer(int value)
{
    if (!isPaused)
    {
        // Cars
        car1X += 0.008f;
        car2X += 0.008f;
        car3X += 0.010f;

        if (car1X > 1.5f)
            car1X = -1.5f;

        if (car2X > 1.5f)
            car2X = -1.5f;

        if (car3X > 1.5f)
            car3X = -1.5f;

        // Buses
        bus1X += 0.0065f;
        bus2X += 0.0075f;

        if (bus1X > 1.5f)
            bus1X = -1.5f;

        if (bus2X > 1.5f)
            bus2X = -1.5f;

        // Wheels
        wheelAngle -= 8.0f;

        if (wheelAngle <= -360.0f)
            wheelAngle = 0.0f;

        // Plane
        // Faster than the cars
        planeX += 0.0065f;

        if (planeX > 1.4f)
            planeX = -1.4f;

        // Clouds
        cloud1X += 0.0010f;
        cloud2X += 0.0006f;
        cloud3X += 0.0014f;

        if (cloud1X > 1.4f)
            cloud1X = -1.4f;

        if (cloud2X > 1.4f)
            cloud2X = -1.4f;

        if (cloud3X > 1.4f)
            cloud3X = -1.4f;

        // Rain
        if (isRaining)
        {
            for (int i = 0; i < MAX_RAIN; i++)
            {
                rainY[i] -= rainSpeed[i];
                rainX[i] -= 0.003f;

                if (rainY[i] < -1.0f)
                {
                    rainY[i] = 1.0f;

                    rainX[i] =
                        ((float)rand() / RAND_MAX)
                        * 2.0f - 1.0f;
                }
            }
        }
    }

    glutPostRedisplay();

    glutTimerFunc(
        16,
        timer,
        0
    );
}


// =================================================================
// KEYBOARD
// =================================================================
void keyboard(
    unsigned char key,
    int x,
    int y
)
{
    if (key == 'n' || key == 'N')
    {
        isNight = true;
    }
    else if (key == 'd' || key == 'D')
    {
        isNight = false;
    }
    else if (key == 'r' || key == 'R')
    {
        isRaining = !isRaining;
    }
    else if (
        key == 'p' ||
        key == 'P' ||
        key == ' '
    )
    {
        isPaused = !isPaused;
    }

    glutPostRedisplay();
}


// =================================================================
// MAIN
// =================================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_SINGLE |
        GLUT_RGB
    );

    glutInitWindowSize(
        1024,
        768
    );

    glutInitWindowPosition(
        100,
        100
    );

    glutCreateWindow(
        "Hazrat Shahjalal International Airport"
    );

    initRain();

    glClearColor(
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    gluOrtho2D(
        -1.0,
        1.0,
        -1.0,
        1.0
    );

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(
        0,
        timer,
        0
    );

    glutMainLoop();

    return 0;
}
