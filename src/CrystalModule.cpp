#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>

#include "CrystalModule.h"
#include "SceneState.h"

namespace crystal {

static void drawPedestal() {
    constexpr int sides = 8;
    constexpr float radius = 0.78f;
    const float stoneColors[][3] = {
        {0.34f, 0.36f, 0.42f}, {0.41f, 0.42f, 0.48f},
        {0.30f, 0.33f, 0.39f}, {0.46f, 0.44f, 0.49f}
    };
    glBegin(GL_TRIANGLES);
    for (int side = 0; side < sides; ++side) {
        const float angle1 = side * 6.2831853f / sides;
        const float angle2 = (side + 1) * 6.2831853f / sides;
        const float x1 = std::cos(angle1) * radius;
        const float z1 = std::sin(angle1) * radius;
        const float x2 = std::cos(angle2) * radius;
        const float z2 = std::sin(angle2) * radius;
        glColor3fv(stoneColors[side % 4]);
        glVertex3f(0.0f, -0.62f, 0.0f);
        glVertex3f(x1, -0.62f, z1);
        glVertex3f(x2, -0.62f, z2);
        glColor3fv(stoneColors[(side + 1) % 4]);
        glVertex3f(x1, -0.62f, z1);
        glVertex3f(x1 * 0.84f, -0.94f, z1 * 0.84f);
        glVertex3f(x2 * 0.84f, -0.94f, z2 * 0.84f);
        glVertex3f(x1, -0.62f, z1);
        glVertex3f(x2 * 0.84f, -0.94f, z2 * 0.84f);
        glVertex3f(x2, -0.62f, z2);
    }
    glEnd();

    const float platformRadius[] = {1.22f, 1.02f};
    const float platformHeight[] = {-0.98f, -0.80f};
    for (int tier = 0; tier < 2; ++tier) {
        glBegin(GL_TRIANGLES);
        for (int side = 0; side < sides; ++side) {
            const float a1 = side * 6.2831853f / sides;
            const float a2 = (side + 1) * 6.2831853f / sides;
            const float x1 = std::cos(a1) * platformRadius[tier];
            const float z1 = std::sin(a1) * platformRadius[tier];
            const float x2 = std::cos(a2) * platformRadius[tier];
            const float z2 = std::sin(a2) * platformRadius[tier];
            glColor3f(0.39f + tier * 0.05f, 0.38f + tier * 0.04f, 0.47f);
            glVertex3f(0.0f, platformHeight[tier], 0.0f);
            glVertex3f(x1, platformHeight[tier], z1);
            glVertex3f(x2, platformHeight[tier], z2);
        }
        glEnd();
    }

    glColor3f(0.22f, 0.42f, 0.24f);
    for (int moss = 0; moss < 4; ++moss) {
        const float angle = moss * 1.57f + 0.35f;
        glPushMatrix();
        glTranslatef(std::cos(angle) * 0.55f, -0.58f, std::sin(angle) * 0.55f);
        glScalef(0.18f, 0.05f, 0.13f);
        glutSolidSphere(1.0f, 8, 5);
        glPopMatrix();
    }
}

static void drawRing(float radius, float height, float tilt, float colorPhase) {
    glPushMatrix();
    glTranslatef(0.0f, height, 0.0f);
    glRotatef(tilt, 1.0f, 0.0f, 0.0f);
    glRotatef(scene_state::elapsedSeconds * 12.0f + colorPhase, 0.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for (int point = 0; point < 48; ++point) {
        const float angle = point * 6.2831853f / 48.0f;
        if (point % 3 == 0) glColor3f(0.98f, 0.82f, 0.42f);
        else if (point % 3 == 1) glColor3f(0.58f, 0.92f, 0.94f);
        else glColor3f(0.82f, 0.68f, 0.98f);
        glVertex3f(std::cos(angle) * radius, 0.0f, std::sin(angle) * radius);
    }
    glEnd();
    glPopMatrix();
}

static void drawFloatingStone(float x, float y, float z, float phase) {
    const float bob = std::sin(scene_state::elapsedSeconds * 1.15f + phase) * 0.08f;
    glPushMatrix();
    glTranslatef(x, y + bob, z);
    glRotatef(scene_state::elapsedSeconds * 14.0f + phase * 20.0f, 0.0f, 1.0f, 0.0f);
    glScalef(0.13f, 0.20f, 0.13f);
    glBegin(GL_TRIANGLES);
    glColor3f(0.54f, 0.72f, 0.78f); glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.8f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.8f);
    glColor3f(0.68f, 0.56f, 0.88f); glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.8f); glVertex3f(0.8f, 0.0f, 0.0f);
    glColor3f(0.48f, 0.62f, 0.72f); glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.8f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -0.8f);
    glColor3f(0.74f, 0.68f, 0.92f); glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -0.8f); glVertex3f(-0.8f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
}

static void drawFlower(float x, float z, float phase) {
    glColor3f(0.28f, 0.56f, 0.36f);
    glBegin(GL_LINES);
    glVertex3f(x, -0.61f, z);
    glVertex3f(x, -0.34f, z);
    glEnd();
    glPushMatrix();
    glTranslatef(x, -0.30f + std::sin(scene_state::elapsedSeconds + phase) * 0.015f, z);
    glColor3f(0.96f, 0.72f + phase * 0.02f, 0.90f);
    glutSolidSphere(0.07f, 8, 6);
    glColor3f(1.0f, 0.88f, 0.56f);
    glutSolidSphere(0.03f, 6, 5);
    glPopMatrix();
}

static void drawPillar(float x, float z, float height) {
    glPushMatrix();
    glTranslatef(x, -0.62f + height * 0.5f, z);
    glColor3f(0.43f, 0.43f, 0.50f);
    glScalef(0.12f, height, 0.12f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x, -0.62f + height + 0.03f, z);
    glColor3f(0.68f, 0.58f, 0.82f);
    glutSolidSphere(0.11f, 8, 6);
    glPopMatrix();
}

static void drawGardenArch(float x, float z, float facing) {
    glPushMatrix();
    glTranslatef(x, -0.62f, z);
    glRotatef(facing, 0.0f, 1.0f, 0.0f);
    for (int side = -1; side <= 1; side += 2) {
        glColor3f(0.48f, 0.46f, 0.54f);
        glPushMatrix();
        glTranslatef(side * 0.42f, 0.30f, 0.0f);
        glScalef(0.14f, 0.62f, 0.14f);
        glutSolidCube(1.0f);
        glPopMatrix();
        glColor3f(0.55f, 0.76f, 0.52f);
        glPushMatrix();
        glTranslatef(side * 0.42f, 0.66f, 0.0f);
        glutSolidSphere(0.09f, 7, 5);
        glPopMatrix();
    }
    glColor3f(0.58f, 0.54f, 0.64f);
    glPushMatrix();
    glTranslatef(0.0f, 0.64f, 0.0f);
    glScalef(0.94f, 0.14f, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();
}

static void drawSanctuarySparkles() {
    glPointSize(3.5f);
    glBegin(GL_POINTS);
    for (int sparkle = 0; sparkle < 14; ++sparkle) {
        const float phase = sparkle * 0.71f;
        const float angle = sparkle * 2.3999632f;
        const float radius = 0.42f + (sparkle % 4) * 0.12f;
        const float glowY = -0.14f + (sparkle % 5) * 0.16f +
                            0.07f * std::sin(scene_state::elapsedSeconds * 1.5f + phase);
        if (sparkle % 3 == 0) glColor3f(0.72f, 0.96f, 1.0f);
        else if (sparkle % 3 == 1) glColor3f(0.98f, 0.82f, 0.50f);
        else glColor3f(0.88f, 0.70f, 1.0f);
        glVertex3f(std::cos(angle) * radius, glowY, std::sin(angle) * radius);
    }
    glEnd();
    glPointSize(1.0f);
}

void drawCrystal() {
    const GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
    const GLboolean blendingWasEnabled = glIsEnabled(GL_BLEND);
    GLfloat previousLineWidth = 1.0f;
    glGetFloatv(GL_LINE_WIDTH, &previousLineWidth);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(-13.0f, -0.64f, 7.5f);
    glScalef(2.10f, 2.10f, 2.10f);
    drawPedestal();
    drawFloatingStone(-0.82f, 0.02f, 0.36f, 0.2f);
    drawFloatingStone(0.70f, 0.20f, 0.46f, 1.8f);
    drawFloatingStone(0.55f, -0.08f, -0.72f, 3.4f);
    drawFloatingStone(-0.62f, 0.30f, -0.48f, 4.6f);
    drawFlower(-0.54f, 0.42f, 0.3f);
    drawFlower(0.58f, -0.28f, 1.6f);
    drawFlower(0.20f, 0.62f, 2.8f);
    drawFlower(-0.92f, -0.12f, 3.8f);
    drawFlower(0.88f, 0.30f, 4.9f);
    drawPillar(-1.05f, -0.78f, 0.62f);
    drawPillar(1.08f, -0.70f, 0.52f);
    drawPillar(-0.18f, 1.08f, 0.48f);
    drawGardenArch(-1.48f, 0.92f, 25.0f);
    drawGardenArch(1.52f, 0.72f, -28.0f);
    for (int stone = 0; stone < 5; ++stone) {
        glColor3f(0.48f, 0.46f, 0.55f);
        glPushMatrix();
        glTranslatef(-0.18f + 0.10f * std::sin(stone), -0.70f, 1.34f + stone * 0.28f);
        glScalef(0.18f, 0.035f, 0.12f);
        glutSolidSphere(1.0f, 7, 5);
        glPopMatrix();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.5f);
    drawRing(0.74f, -0.24f, 12.0f, 0.0f);
    drawRing(0.52f, 0.38f, -18.0f, 90.0f);
    drawSanctuarySparkles();
    glDisable(GL_BLEND);

    glPushMatrix();
    glRotatef(scene_state::crystalAngle, 0.0f, 1.0f, 0.0f);

    const GLfloat upperTip[] = {0.0f, 1.15f, 0.0f};
    const GLfloat lowerTip[] = {0.0f, -0.60f, 0.0f};
    const GLfloat ring[][3] = {
        {0.42f, 0.0f, 0.0f},
        {0.21f, 0.0f, 0.36f},
        {-0.21f, 0.0f, 0.36f},
        {-0.42f, 0.0f, 0.0f},
        {-0.21f, 0.0f, -0.36f},
        {0.21f, 0.0f, -0.36f}
    };
    const GLfloat faceColors[][3] = {
        {0.30f, 0.90f, 1.00f},
        {0.72f, 0.58f, 0.98f},
        {0.58f, 0.72f, 1.00f},
        {0.62f, 0.35f, 0.88f},
        {0.78f, 0.88f, 1.00f},
        {0.98f, 0.80f, 0.36f}
    };

    glBegin(GL_TRIANGLES);
    for (int side = 0; side < 6; ++side) {
        const int next = (side + 1) % 6;
        glColor3fv(faceColors[side]);
        glVertex3fv(upperTip);
        glVertex3fv(ring[side]);
        glVertex3fv(ring[next]);
    }
    for (int side = 0; side < 6; ++side) {
        const int next = (side + 1) % 6;
        glColor3fv(faceColors[(side + 2) % 6]);
        glVertex3fv(lowerTip);
        glVertex3fv(ring[next]);
        glVertex3fv(ring[side]);
    }
    glEnd();

    glPopMatrix();
    glPopMatrix();

    if (lightingWasEnabled) glEnable(GL_LIGHTING);
    if (blendingWasEnabled) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    glLineWidth(previousLineWidth);
}

}
