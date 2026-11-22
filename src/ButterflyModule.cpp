#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>

#include "ButterflyModule.h"
#include "CameraModule.h"
#include "SceneState.h"

const GLfloat upperWingVertices[] = {
    0.24f, 0.34f, 0.10f,
    0.00f, -0.08f, 0.03f, 0.10f, 0.48f, 0.12f,
    0.36f, 0.96f, 0.22f, 0.74f, 1.27f, 0.28f,
    1.15f, 1.24f, 0.24f, 1.48f, 1.02f, 0.16f,
    1.62f, 0.66f, 0.07f, 1.49f, 0.31f, 0.02f,
    1.12f, 0.04f, -0.01f, 0.67f, -0.16f, 0.00f,
    0.25f, -0.24f, 0.02f, 0.00f, -0.08f, 0.03f
};

const GLfloat upperWingColors[] = {
    0.52f, 0.88f, 0.95f,
    0.64f, 0.72f, 0.98f, 0.61f, 0.91f, 0.96f,
    0.74f, 0.67f, 0.98f, 0.87f, 0.76f, 1.00f,
    0.96f, 0.76f, 0.92f, 0.95f, 0.82f, 0.99f,
    0.70f, 0.91f, 1.00f, 0.54f, 0.86f, 0.93f,
    0.77f, 0.66f, 0.96f, 0.90f, 0.74f, 0.96f,
    0.62f, 0.80f, 0.98f, 0.64f, 0.72f, 0.98f
};

const GLfloat lowerWingVertices[] = {
    0.20f, -0.28f, 0.07f,
    0.00f, -0.16f, 0.03f, 0.40f, -0.13f, 0.08f,
    0.80f, -0.33f, 0.14f, 1.08f, -0.68f, 0.12f,
    0.96f, -1.06f, 0.07f, 0.71f, -1.27f, 0.02f,
    0.39f, -1.02f, 0.01f, 0.14f, -0.60f, 0.02f,
    0.00f, -0.16f, 0.03f
};

const GLfloat lowerWingColors[] = {
    0.60f, 0.78f, 0.98f,
    0.71f, 0.66f, 0.96f, 0.88f, 0.72f, 0.96f,
    0.99f, 0.76f, 0.88f, 0.74f, 0.88f, 1.00f,
    0.56f, 0.85f, 0.94f, 0.76f, 0.66f, 0.96f,
    0.94f, 0.78f, 0.98f, 0.58f, 0.86f, 0.92f,
    0.71f, 0.66f, 0.96f
};

const GLfloat tailVertices[] = {
    0.52f, -0.86f, 0.08f, 0.90f, -1.06f, 0.14f, 0.63f, -1.50f, 0.10f,
    0.90f, -1.06f, 0.14f, 0.82f, -1.55f, 0.09f, 0.63f, -1.50f, 0.10f,
    0.63f, -1.50f, 0.10f, 0.82f, -1.55f, 0.09f, 0.54f, -2.04f, 0.03f,
    0.82f, -1.55f, 0.09f, 0.70f, -2.34f, -0.05f, 0.54f, -2.04f, 0.03f,
    0.54f, -2.04f, 0.03f, 0.70f, -2.34f, -0.05f, 0.42f, -2.17f, -0.02f
};

const GLfloat tailColors[] = {
    0.84f, 0.70f, 0.98f, 0.96f, 0.76f, 0.88f, 0.62f, 0.88f, 0.96f,
    0.96f, 0.76f, 0.88f, 0.72f, 0.82f, 1.00f, 0.62f, 0.88f, 0.96f,
    0.62f, 0.88f, 0.96f, 0.72f, 0.82f, 1.00f, 0.96f, 0.72f, 0.94f,
    0.72f, 0.82f, 1.00f, 0.96f, 0.78f, 0.90f, 0.96f, 0.72f, 0.94f,
    0.96f, 0.72f, 0.94f, 0.98f, 0.86f, 0.56f, 0.88f, 0.70f, 0.96f
};

const GLfloat wingMarkVertices[] = {
    0.26f, 0.22f, 0.16f, 0.57f, 0.69f, 0.25f, 0.47f, 0.25f, 0.16f,
    0.60f, 0.44f, 0.20f, 1.08f, 0.62f, 0.24f, 0.76f, 0.19f, 0.12f,
    0.28f, -0.39f, 0.12f, 0.66f, -0.61f, 0.18f, 0.40f, -0.82f, 0.12f
};

const GLfloat wingMarkColors[] = {
    0.98f, 0.88f, 0.48f, 0.94f, 0.92f, 1.00f, 0.96f, 0.70f, 0.94f,
    0.96f, 0.74f, 0.94f, 0.99f, 0.88f, 0.54f, 0.78f, 0.94f, 1.00f,
    0.98f, 0.86f, 0.56f, 0.74f, 0.92f, 0.98f, 0.96f, 0.72f, 0.94f
};

const GLfloat wingVeinVertices[] = {
    0.08f, 0.02f, 0.10f, 0.42f, 0.84f, 0.25f,
    0.08f, 0.02f, 0.10f, 1.16f, 0.88f, 0.23f,
    0.08f, 0.02f, 0.10f, 1.28f, 0.34f, 0.10f,
    0.10f, -0.22f, 0.08f, 0.83f, -0.54f, 0.18f,
    0.10f, -0.22f, 0.08f, 0.72f, -1.12f, 0.08f,
    0.56f, -1.00f, 0.14f, 0.58f, -2.09f, 0.07f
};

const GLfloat upperEdgeVertices[] = {
    0.00f, -0.08f, 0.05f, 0.10f, 0.48f, 0.14f,
    0.36f, 0.96f, 0.24f, 0.74f, 1.27f, 0.30f,
    1.15f, 1.24f, 0.26f, 1.48f, 1.02f, 0.18f,
    1.62f, 0.66f, 0.09f, 1.49f, 0.31f, 0.04f,
    1.12f, 0.04f, 0.01f, 0.67f, -0.16f, 0.02f,
    0.25f, -0.24f, 0.04f, 0.00f, -0.08f, 0.05f
};

const GLfloat lowerEdgeVertices[] = {
    0.00f, -0.16f, 0.05f, 0.40f, -0.13f, 0.10f,
    0.80f, -0.33f, 0.16f, 1.08f, -0.68f, 0.14f,
    0.96f, -1.06f, 0.09f, 0.71f, -1.27f, 0.04f,
    0.39f, -1.02f, 0.03f, 0.14f, -0.60f, 0.04f,
    0.00f, -0.16f, 0.05f
};

const GLfloat tailEdgeVertices[] = {
    0.52f, -0.86f, 0.10f, 0.90f, -1.06f, 0.16f,
    0.82f, -1.55f, 0.11f, 0.70f, -2.34f, -0.03f,
    0.42f, -2.17f, 0.00f, 0.54f, -2.04f, 0.05f,
    0.63f, -1.50f, 0.12f, 0.52f, -0.86f, 0.10f
};

const GLfloat antennaVertices[] = {
    -0.07f, 0.48f, 0.08f, -0.27f, 0.83f, 0.12f,
     0.07f, 0.48f, 0.08f,  0.27f, 0.83f, 0.12f
};

static void drawColoredArray(GLenum mode, const GLfloat* vertices,
    const GLfloat* colors, GLsizei count) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(mode, 0, count);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

static void drawLineArray(GLenum mode, const GLfloat* vertices, GLsizei count) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(mode, 0, count);
    glDisableClientState(GL_VERTEX_ARRAY);
}

static void drawWing(bool mirror, float wingAngle, float tailAngle) {
    glPushMatrix();
    if (mirror) glScalef(-1.0f, 1.0f, 1.0f);
    glRotatef(wingAngle, 0.0f, 1.0f, 0.0f);

    drawColoredArray(GL_TRIANGLE_FAN, upperWingVertices, upperWingColors, 13);
    drawColoredArray(GL_TRIANGLE_FAN, lowerWingVertices, lowerWingColors, 10);
    drawColoredArray(GL_TRIANGLES, wingMarkVertices, wingMarkColors, 9);

    glColor3f(0.98f, 0.94f, 1.0f);
    glLineWidth(1.5f);
    drawLineArray(GL_LINE_STRIP, upperEdgeVertices, 12);
    drawLineArray(GL_LINE_STRIP, lowerEdgeVertices, 9);

    glColor3f(0.98f, 0.86f, 0.56f);
    glLineWidth(1.0f);
    drawLineArray(GL_LINES, wingVeinVertices, 12);

    glPushMatrix();
    glTranslatef(0.52f, -0.86f, 0.08f);
    glRotatef(mirror ? -tailAngle : tailAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-0.52f, 0.86f, -0.08f);
    drawColoredArray(GL_TRIANGLES, tailVertices, tailColors, 15);
    glColor3f(0.98f, 0.94f, 1.0f);
    glLineWidth(1.5f);
    drawLineArray(GL_LINE_STRIP, tailEdgeVertices, 8);
    glPopMatrix();
    glPopMatrix();
}

static void drawBody() {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glPushMatrix();
    glTranslatef(0.0f, -0.18f, 0.09f);
    glScalef(0.17f, 0.70f, 0.22f);
    glColor3f(0.30f, 0.24f, 0.48f);
    glutSolidSphere(1.0f, 12, 8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.53f, 0.10f);
    glScalef(0.16f, 0.16f, 0.16f);
    glColor3f(0.92f, 0.76f, 0.52f);
    glutSolidSphere(1.0f, 10, 8);
    glPopMatrix();

    glDisable(GL_LIGHTING);
}

namespace butterfly {

    void drawButterfly(float x, float y, float z, float yaw, float pitch) {
        const GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
        const GLboolean colorMaterialWasEnabled = glIsEnabled(GL_COLOR_MATERIAL);
        GLfloat previousLineWidth = 1.0f;
        GLint previousArrayBuffer = 0;
        glGetFloatv(GL_LINE_WIDTH, &previousLineWidth);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArrayBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisable(GL_LIGHTING);

        const float wingAngle = 16.0f * std::sin(scene_state::elapsedSeconds * 4.0f);
        const float tailAngle = 6.0f * std::sin(scene_state::elapsedSeconds * 3.20f + 0.80f);
        const float hoverBob = 0.045f * std::sin(scene_state::elapsedSeconds * 2.10f);
        static float visualPitch = -18.0f;
        static float visualBank = 0.0f;
        static float previousAnimationTime = -1.0f;
        static float previousYaw = yaw;
        float animationDelta = 0.0f;
        if (previousAnimationTime >= 0.0f) {
            animationDelta = std::fmin(0.05f,
                scene_state::elapsedSeconds - previousAnimationTime);
        }
        previousAnimationTime = scene_state::elapsedSeconds;
        float pitchTarget = -18.0f;
        if (camera::isForwardInputActive()) pitchTarget = -90.0f + pitch;
        if (camera::isReturningToGround()) pitchTarget = -108.0f;
        float yawChange = yaw - previousYaw;
        while (yawChange > 180.0f) yawChange -= 360.0f;
        while (yawChange < -180.0f) yawChange += 360.0f;
        previousYaw = yaw;
        float bankTarget = std::fmax(-8.0f, std::fmin(8.0f, -yawChange * 0.8f));
        if (scene_state::keys['a'] || scene_state::keys['A']) bankTarget += 6.0f;
        if (scene_state::keys['d'] || scene_state::keys['D']) bankTarget -= 6.0f;
        bankTarget = std::fmax(-10.0f, std::fmin(10.0f, bankTarget));
        const float postureResponse = 1.0f - std::exp(-5.0f * animationDelta);
        visualPitch += (pitchTarget - visualPitch) * postureResponse;
        visualBank += (bankTarget - visualBank) * postureResponse;
        glPushMatrix();
        glTranslatef(x, y + hoverBob, z);
        glRotatef(-yaw - 90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(visualPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(visualBank, 0.0f, 0.0f, 1.0f);
        glScalef(scene_state::butterflyScale, scene_state::butterflyScale,
            scene_state::butterflyScale);
        drawWing(false, wingAngle, tailAngle);
        drawWing(true, wingAngle, tailAngle);
        drawBody();

        glColor3f(0.38f, 0.25f, 0.54f);
        glLineWidth(1.5f);
        drawLineArray(GL_LINES, antennaVertices, 4);
        glPopMatrix();

        if (lightingWasEnabled) glEnable(GL_LIGHTING);
        if (colorMaterialWasEnabled) glEnable(GL_COLOR_MATERIAL);
        else glDisable(GL_COLOR_MATERIAL);
        glLineWidth(previousLineWidth);
        glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(previousArrayBuffer));
    }

}
