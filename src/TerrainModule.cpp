#include <GL/glew.h>
#include <GL/glut.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "RenderingModule.h"
#include "SceneState.h"
#include "TerrainModule.h"

namespace terrain {

void drawHemisphere(float centerX, float centerZ, float radius, const Color& color) {
    rendering::setMaterial(color, {0.07f, 0.15f, 0.025f}, 3.0f);
    glPushMatrix();
    glTranslatef(centerX, -2.75f, centerZ);
    glScalef(radius, radius, radius);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, scene_state::moundVBO);
    glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
    glNormalPointer(GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glColorPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glDrawArrays(GL_TRIANGLES, 0, scene_state::moundVertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();

}

void drawLandmass() {
    rendering::setMaterial({0.24f, 0.43f, 0.12f}, {0.045f, 0.10f, 0.018f}, 2.0f);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, scene_state::landVBO);
    glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
    glNormalPointer(GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glColorPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glDrawArrays(GL_TRIANGLES, 0, scene_state::landVertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
}

void drawRocks() {
    if (scene_state::rockVertexCount == 0) return;
    rendering::setMaterial({0.32f, 0.30f, 0.26f}, {0.08f, 0.075f, 0.06f}, 4.0f);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, scene_state::rockVBO);
    glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
    glNormalPointer(GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glColorPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glDrawArrays(GL_TRIANGLES, 0, scene_state::rockVertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
}

void drawProps() {
    if (scene_state::propVertexCount == 0) return;
    rendering::setMaterial({0.34f, 0.38f, 0.16f}, {0.07f, 0.08f, 0.035f}, 5.0f);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, scene_state::propVBO);
    glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
    glNormalPointer(GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glColorPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glDrawArrays(GL_TRIANGLES, 0, scene_state::propVertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
}

void drawGrass() {
    rendering::setMaterial({0.20f, 0.58f, 0.10f}, {0.035f, 0.12f, 0.018f}, 2.0f);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, scene_state::grassVBO);
    glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
    glNormalPointer(GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glColorPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glDrawArrays(GL_TRIANGLES, 0, scene_state::grassVertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
}

static float scenicGroundHeight(float x, float z) {
    const float distance = std::sqrt(x * x + z * z);
    const float patchNoise = std::sin(x * 0.62f) + std::cos(z * 0.48f) +
                             0.55f * std::sin((x - z) * 0.35f);
    const float patchEdgeDip = std::max(0.0f, std::min(1.0f, (0.25f - patchNoise) / 0.60f));
    const float lakeEdgeDip = std::max(0.0f, std::min(1.0f, (5.20f - distance)));
    const float outerBasin = std::max(0.0f, std::min(1.0f, (distance - 11.0f) / 7.0f));
    return -2.67f + 0.16f * std::sin(x * 1.35f) + 0.12f * std::cos(z * 1.10f) +
           0.07f * std::sin((x + z) * 2.20f) - 0.18f * patchEdgeDip -
           0.24f * lakeEdgeDip - 0.30f * outerBasin;
}

static void drawScenicFlower(float x, float z, float colorShift) {
    const float y = scenicGroundHeight(x, z);
    glColor3f(0.20f, 0.56f, 0.22f);
    glBegin(GL_LINES);
    glVertex3f(x, y, z);
    glVertex3f(x, y + 0.22f, z);
    glEnd();
    glPushMatrix();
    glTranslatef(x, y + 0.23f, z);
    for (int petal = 0; petal < 5; ++petal) {
        const float angle = petal * 1.2566371f;
        glPushMatrix();
        glTranslatef(std::cos(angle) * 0.055f, 0.0f, std::sin(angle) * 0.055f);
        glColor3f(0.88f + colorShift * 0.08f, 0.62f + colorShift * 0.10f, 0.92f);
        glutSolidSphere(0.052f, 6, 5);
        glPopMatrix();
    }
    glColor3f(1.0f, 0.82f, 0.28f);
    glutSolidSphere(0.038f, 6, 5);
    glPopMatrix();
}

static void drawScenicBush(float x, float z, float size) {
    const float y = scenicGroundHeight(x, z);
    glColor3f(0.16f, 0.40f, 0.16f);
    glPushMatrix();
    glTranslatef(x, y + size * 0.26f, z);
    glScalef(size, size * 0.58f, size * 0.78f);
    glutSolidSphere(1.0f, 7, 5);
    glPopMatrix();
    glColor3f(0.29f, 0.58f, 0.20f);
    glPushMatrix();
    glTranslatef(x + size * 0.22f, y + size * 0.42f, z - size * 0.10f);
    glScalef(size * 0.70f, size * 0.48f, size * 0.60f);
    glutSolidSphere(1.0f, 7, 5);
    glPopMatrix();
}

static void drawScenicTree(float x, float z, float size) {
    const float y = scenicGroundHeight(x, z);
    const float branchSway = std::sin(scene_state::elapsedSeconds * 0.75f + x * 0.3f) * 4.0f;
    glColor3f(0.34f, 0.22f, 0.20f);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(size * 0.18f, size * 0.94f, size * 0.18f);
    glutSolidCube(1.0f);
    glPopMatrix();

    const float canopy[][4] = {
        {0.0f, 1.00f, 0.0f, 0.62f}, {-0.36f, 0.90f, 0.08f, 0.46f},
        {0.38f, 0.92f, -0.05f, 0.50f}, {-0.12f, 1.30f, 0.08f, 0.42f},
        {0.27f, 1.24f, 0.10f, 0.40f}
    };
    for (int puff = 0; puff < 5; ++puff) {
        const float sway = branchSway * (0.4f + puff * 0.1f);
        if (puff % 3 == 0) glColor3f(0.96f, 0.62f, 0.78f);
        else if (puff % 3 == 1) glColor3f(0.84f, 0.66f, 0.94f);
        else glColor3f(1.0f, 0.76f, 0.70f);
        glPushMatrix();
        glTranslatef(x + canopy[puff][0] * size, y + canopy[puff][1] * size, z + canopy[puff][2] * size);
        glRotatef(sway, 0.0f, 0.0f, 1.0f);
        glScalef(canopy[puff][3] * size, canopy[puff][3] * size * 0.72f, canopy[puff][3] * size);
        glutSolidSphere(1.0f, 9, 7);
        glPopMatrix();
    }
}

static void drawMushroomGrove(float x, float z, float phase) {
    const float y = scenicGroundHeight(x, z);
    for (int mushroom = 0; mushroom < 5; ++mushroom) {
        const float angle = mushroom * 1.2566371f + phase;
        const float distance = 0.14f + mushroom * 0.09f;
        const float size = 0.10f + (mushroom % 3) * 0.045f;
        const float mx = x + std::cos(angle) * distance;
        const float mz = z + std::sin(angle) * distance;
        const float pulse = 0.5f + 0.5f * std::sin(scene_state::elapsedSeconds * 1.8f + phase + mushroom);
        glColor3f(0.72f, 0.72f, 0.82f);
        glPushMatrix();
        glTranslatef(mx, y + size * 0.55f, mz);
        glScalef(size * 0.38f, size * 1.1f, size * 0.38f);
        glutSolidCube(1.0f);
        glPopMatrix();
        if (mushroom % 3 == 0) glColor3f(0.46f + pulse * 0.20f, 0.82f, 1.0f);
        else if (mushroom % 3 == 1) glColor3f(0.82f, 0.52f + pulse * 0.18f, 0.96f);
        else glColor3f(0.96f, 0.62f, 0.82f + pulse * 0.10f);
        glPushMatrix();
        glTranslatef(mx, y + size * 1.18f, mz);
        glScalef(size, size * 0.42f, size);
        glutSolidSphere(1.0f, 8, 5);
        glPopMatrix();
    }
}

static void drawFireflyCluster(float homeX, float homeZ, int count, float phase) {
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int firefly = 0; firefly < count; ++firefly) {
        const float localPhase = phase + firefly * 0.79f;
        const float time = scene_state::elapsedSeconds * (0.65f + (firefly % 3) * 0.06f) + localPhase;
        const float x = homeX + (firefly % 4 - 1.5f) * 0.34f + 0.20f * std::sin(time);
        const float z = homeZ + (firefly / 4 - 0.8f) * 0.38f + 0.17f * std::cos(time * 1.16f);
        const float glow = 0.52f + 0.38f * (0.5f + 0.5f * std::sin(time * 2.2f));
        if (firefly % 4 == 0) glColor4f(0.54f, 0.90f, 1.0f, glow);
        else if (firefly % 4 == 1) glColor4f(1.0f, 0.84f, 0.50f, glow);
        else if (firefly % 4 == 2) glColor4f(0.86f, 0.76f, 1.0f, glow);
        else glColor4f(0.96f, 0.98f, 0.92f, glow);
        glVertex3f(x, scenicGroundHeight(x, z) + 0.36f + 0.15f * std::sin(time * 1.35f), z);
    }
    glEnd();
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    const float time = scene_state::elapsedSeconds * 0.72f + phase;
    glColor4f(0.82f, 0.96f, 1.0f, 0.72f);
    glVertex3f(homeX + 0.22f * std::sin(time),
               scenicGroundHeight(homeX, homeZ) + 0.62f + 0.14f * std::cos(time * 1.2f),
               homeZ + 0.18f * std::cos(time));
    glEnd();
    glPointSize(1.0f);
}

static void drawCelestialGateway() {
    constexpr float gatewayX = 14.0f;
    constexpr float gatewayZ = -8.0f;
    const float groundY = scenicGroundHeight(gatewayX, gatewayZ);
    const GLboolean blendingWasEnabled = glIsEnabled(GL_BLEND);

    glPushMatrix();
    glTranslatef(gatewayX, groundY, gatewayZ);

    // Three broad platform levels make the monument read from flight distance.
    for (int step = 0; step < 3; ++step) {
        const float width = 3.30f - step * 0.42f;
        glColor3f(0.40f + step * 0.035f, 0.40f + step * 0.025f, 0.50f + step * 0.025f);
        glPushMatrix();
        glTranslatef(0.0f, 0.08f + step * 0.13f, 0.0f);
        glScalef(width, 0.16f, 2.45f - step * 0.32f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // A segmented full stone ring provides a clear ancient-gateway silhouette.
    constexpr int ringBlocks = 18;
    constexpr float ringRadius = 1.28f;
    for (int block = 0; block < ringBlocks; ++block) {
        const float angle = block * 6.2831853f / ringBlocks;
        const float x = std::cos(angle) * ringRadius;
        const float y = 1.83f + std::sin(angle) * ringRadius;
        if (block % 4 == 0) glColor3f(0.58f, 0.56f, 0.68f);
        else glColor3f(0.43f, 0.44f, 0.54f);
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glRotatef(-angle * 57.29578f, 0.0f, 0.0f, 1.0f);
        glScalef(0.34f, 0.20f, 0.26f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Gold capstones and a restrained rune line distinguish it from the crystal.
    glColor3f(0.96f, 0.76f, 0.34f);
    for (int rune = 0; rune < 4; ++rune) {
        const float angle = 0.55f + rune * 1.35f;
        glPushMatrix();
        glTranslatef(std::cos(angle) * 1.42f, 1.83f + std::sin(angle) * 1.42f, 0.03f);
        glScalef(0.10f, 0.10f, 0.05f);
        glutSolidSphere(1.0f, 6, 5);
        glPopMatrix();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const float energyPulse = 0.74f + 0.16f * std::sin(scene_state::elapsedSeconds * 1.4f);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.48f, 0.90f, 1.0f, 0.18f);
    glVertex3f(0.0f, 1.83f, -0.03f);
    for (int point = 0; point <= 24; ++point) {
        const float angle = point * 6.2831853f / 24.0f;
        glColor4f(0.70f, 0.68f, 1.0f, 0.32f);
        glVertex3f(std::cos(angle) * 0.88f, 1.83f + std::sin(angle) * 1.14f * energyPulse, -0.03f);
    }
    glEnd();
    glColor4f(0.76f, 0.96f, 1.0f, 0.70f);
    glBegin(GL_LINE_LOOP);
    for (int point = 0; point < 28; ++point) {
        const float angle = point * 6.2831853f / 28.0f;
        glVertex3f(std::cos(angle) * 0.90f, 1.83f + std::sin(angle) * 1.16f * energyPulse, -0.05f);
    }
    glEnd();

    // Four slow stones orbit without becoming a second crystal monument.
    for (int stone = 0; stone < 4; ++stone) {
        const float angle = scene_state::elapsedSeconds * 0.35f + stone * 1.5707963f;
        glColor4f(stone % 2 ? 0.66f : 0.90f, 0.84f, 1.0f, 0.78f);
        glPushMatrix();
        glTranslatef(std::cos(angle) * 1.62f, 1.80f + 0.24f * std::sin(angle * 1.5f),
                     std::sin(angle) * 0.38f);
        glScalef(0.10f, 0.18f, 0.10f);
        glutSolidSphere(1.0f, 6, 5);
        glPopMatrix();
    }
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (int sparkle = 0; sparkle < 12; ++sparkle) {
        const float phase = sparkle * 0.81f;
        const float time = scene_state::elapsedSeconds * 0.75f + phase;
        glColor4f(sparkle % 2 ? 0.94f : 0.60f, 0.88f, 1.0f, 0.50f);
        glVertex3f((sparkle % 4 - 1.5f) * 0.28f + 0.12f * std::sin(time),
                   0.58f + (sparkle / 4) * 0.54f + 0.10f * std::cos(time),
                   0.12f * std::cos(time * 1.1f));
    }
    glEnd();
    glPointSize(1.0f);
    if (!blendingWasEnabled) glDisable(GL_BLEND);

    // A garden frame and a short curved approach integrate the clearing.
    for (int flower = 0; flower < 10; ++flower) {
        const float angle = flower * 6.2831853f / 10.0f;
        glPushMatrix();
        glTranslatef(std::cos(angle) * 1.55f, 0.28f, std::sin(angle) * 1.12f);
        if (flower % 3 == 0) glColor3f(0.96f, 0.66f, 0.84f);
        else if (flower % 3 == 1) glColor3f(0.76f, 0.72f, 0.98f);
        else glColor3f(0.62f, 0.94f, 0.94f);
        glutSolidSphere(0.075f, 7, 5);
        glPopMatrix();
    }
    for (int pathStone = 0; pathStone < 6; ++pathStone) {
        const float progress = pathStone / 5.0f;
        glColor3f(0.48f, 0.47f, 0.56f);
        glPushMatrix();
        glTranslatef(-1.8f + progress * 1.25f, 0.12f, 1.55f - progress * 0.82f);
        glRotatef(pathStone * 18.0f, 0.0f, 1.0f, 0.0f);
        glScalef(0.28f, 0.05f, 0.18f);
        glutSolidSphere(1.0f, 7, 5);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawScenicDetails() {
    const GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHTING);

    const float flowerPatches[][2] = {
        {-10.8f, 6.5f}, {-11.6f, 8.2f}, {-8.7f, 9.3f},
        {7.7f, 7.6f}, {9.2f, 6.1f}, {10.6f, -5.8f},
        {-7.2f, -8.5f}, {-10.1f, -6.7f}
    };
    for (int patch = 0; patch < 8; ++patch) {
        for (int flower = 0; flower < 5; ++flower) {
            const float angle = flower * 1.2566371f + patch * 0.37f;
            drawScenicFlower(flowerPatches[patch][0] + std::cos(angle) * 0.34f,
                             flowerPatches[patch][1] + std::sin(angle) * 0.24f,
                             static_cast<float>((patch + flower) % 3) * 0.35f);
        }
    }

    drawScenicBush(-8.8f, 7.1f, 0.46f);
    drawScenicBush(-9.8f, -6.5f, 0.55f);
    drawScenicBush(8.8f, 6.6f, 0.52f);
    drawScenicBush(11.2f, -5.4f, 0.42f);
    // Large, layered blossom trees establish the crystal garden and meadow groves.
    drawScenicTree(-14.8f, 8.8f, 1.65f);
    drawScenicTree(-11.2f, 10.2f, 1.42f);
    drawScenicTree(-9.0f, 8.9f, 1.20f);
    drawScenicTree(12.8f, 5.2f, 1.48f);
    drawScenicTree(10.6f, -8.7f, 1.24f);
    drawScenicTree(-11.6f, -7.8f, 1.10f);
    drawMushroomGrove(-11.4f, 6.1f, 0.2f);
    drawMushroomGrove(-7.5f, 7.0f, 1.5f);
    drawMushroomGrove(8.4f, -6.4f, 2.7f);
    drawMushroomGrove(10.2f, 6.3f, 4.1f);
    drawCelestialGateway();

    // A curved route leads from the lake meadow toward the crystal garden.
    for (int stone = 0; stone < 9; ++stone) {
        const float progress = stone / 8.0f;
        const float x = -7.4f - progress * 4.7f;
        const float z = 5.1f + progress * 2.1f + 0.22f * std::sin(stone * 1.7f);
        const float y = scenicGroundHeight(x, z);
        glColor3f(0.48f, 0.46f, 0.54f);
        glPushMatrix();
        glTranslatef(x, y + 0.035f, z);
        glRotatef(stone * 23.0f, 0.0f, 1.0f, 0.0f);
        glScalef(0.27f, 0.045f, 0.18f);
        glutSolidSphere(1.0f, 7, 5);
        glPopMatrix();
    }

    const float ruins[][2] = {{-7.0f, 7.8f}, {8.9f, -6.7f}};
    for (int ruin = 0; ruin < 2; ++ruin) {
        const float baseY = scenicGroundHeight(ruins[ruin][0], ruins[ruin][1]);
        for (int side = -1; side <= 1; side += 2) {
            glColor3f(0.43f, 0.42f, 0.49f);
            glPushMatrix();
            glTranslatef(ruins[ruin][0] + side * 0.28f, baseY + 0.22f, ruins[ruin][1]);
            glScalef(0.13f, 0.44f, 0.13f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
        glColor3f(0.50f, 0.48f, 0.58f);
        glPushMatrix();
        glTranslatef(ruins[ruin][0], baseY + 0.42f, ruins[ruin][1]);
        glScalef(0.70f, 0.10f, 0.15f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    const GLboolean blendingWasEnabled = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawFireflyCluster(-11.5f, 7.0f, 10, 0.2f);  // Crystal garden.
    drawFireflyCluster(-8.7f, 8.6f, 7, 1.4f);   // Blossom grove.
    drawFireflyCluster(-7.4f, 6.9f, 6, 2.6f);   // Mushroom grove.
    drawFireflyCluster(8.8f, -6.2f, 8, 3.7f);   // Meadow ruins.
    drawFireflyCluster(7.4f, 4.7f, 6, 5.0f);    // Lake shore.
    drawFireflyCluster(13.3f, -7.2f, 8, 6.3f);  // Celestial Gateway clearing.
    if (!blendingWasEnabled) glDisable(GL_BLEND);
    if (lightingWasEnabled) glEnable(GL_LIGHTING);
}

void drawMound() {
    drawHemisphere(0.0f, 0.0f, 2.35f, {0.32f, 0.56f, 0.14f});
    drawHemisphere(-2.85f, -0.62f, 1.35f, {0.25f, 0.48f, 0.12f});
    drawHemisphere(2.65f, -0.38f, 1.15f, {0.38f, 0.62f, 0.16f});
    drawHemisphere(1.10f, 2.05f, 0.88f, {0.29f, 0.52f, 0.13f});
    drawHemisphere(0.0f, -2.35f, 0.72f, {0.24f, 0.45f, 0.11f});

    // Smaller outlying mounds break up the open meadow around the center.
    drawHemisphere(-5.80f, -3.20f, 0.58f, {0.20f, 0.40f, 0.10f});
    drawHemisphere(4.85f, -4.35f, 0.74f, {0.34f, 0.54f, 0.13f});
    drawHemisphere(-5.15f, 3.75f, 0.66f, {0.27f, 0.47f, 0.11f});
    drawHemisphere(4.65f, 4.90f, 0.52f, {0.42f, 0.58f, 0.15f});
    drawHemisphere(7.00f, -0.90f, 0.46f, {0.24f, 0.43f, 0.09f});
    drawHemisphere(-7.10f, 1.35f, 0.55f, {0.30f, 0.49f, 0.12f});
    drawHemisphere(1.90f, 5.95f, 0.44f, {0.38f, 0.55f, 0.14f});
    drawHemisphere(-2.85f, -6.05f, 0.40f, {0.22f, 0.38f, 0.08f});
}

}
