#include <GL/glew.h>
#include <GL/glut.h>

#include <algorithm>
#include <cmath>

#include "FloatingIslandsModule.h"
#include "RenderingModule.h"
#include "SceneState.h"

namespace floating_islands {

    void drawFloatingIsland(float x, float y, float z, float radius, float phase);

    static void drawFountainBasin(float radius, float height) {
        constexpr int sides = 12;
        glBegin(GL_TRIANGLES);
        for (int side = 0; side < sides; ++side) {
            const float a1 = side * 6.2831853f / sides;
            const float a2 = (side + 1) * 6.2831853f / sides;
            const float x1 = std::cos(a1) * radius;
            const float z1 = std::sin(a1) * radius;
            const float x2 = std::cos(a2) * radius;
            const float z2 = std::sin(a2) * radius;
            glColor3f(0.42f, 0.43f, 0.50f);
            glVertex3f(0.0f, height, 0.0f);
            glVertex3f(x1, height, z1);
            glVertex3f(x2, height, z2);
            glColor3f(0.30f, 0.32f, 0.40f);
            glVertex3f(x1, height, z1);
            glVertex3f(x1 * 0.84f, height - 0.13f, z1 * 0.84f);
            glVertex3f(x2 * 0.84f, height - 0.13f, z2 * 0.84f);
            glVertex3f(x1, height, z1);
            glVertex3f(x2 * 0.84f, height - 0.13f, z2 * 0.84f);
            glVertex3f(x2, height, z2);
        }
        glEnd();

        glColor3f(0.78f, 0.64f, 0.94f);
        glBegin(GL_LINE_LOOP);
        for (int side = 0; side < sides; ++side) {
            const float angle = side * 6.2831853f / sides;
            glVertex3f(std::cos(angle) * radius, height + 0.012f,
                std::sin(angle) * radius);
        }
        glEnd();
    }

    static void drawFountainWater(float radius, float height, float phase) {
        constexpr int sides = 16;
        const float ripple = 0.025f * std::sin(scene_state::elapsedSeconds * 2.5f + phase);
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(0.82f, 0.96f, 1.0f, 0.82f);
        glVertex3f(0.0f, height + 0.025f, 0.0f);
        for (int side = 0; side <= sides; ++side) {
            const float angle = side * 6.2831853f / sides;
            const float edge = radius * (0.82f + ripple * std::sin(angle * 3.0f));
            glColor4f(0.36f, 0.82f, 0.98f, 0.68f);
            glVertex3f(std::cos(angle) * edge, height + 0.025f,
                std::sin(angle) * edge);
        }
        glEnd();
    }

    static void drawWaterCurtains(float radius, float topY, float bottomY, float phase) {
        const int curtains = radius > 0.50f ? 4 : 3;
        const float drop = topY - bottomY;
        const float curtainWidth = radius > 0.50f ? 0.13f : 0.085f;
        for (int stream = 0; stream < curtains; ++stream) {
            const float angle = stream * 6.2831853f / curtains + phase;
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            const float sideX = -std::sin(angle) * curtainWidth;
            const float sideZ = std::cos(angle) * curtainWidth;
            glBegin(GL_TRIANGLE_STRIP);
            glColor4f(0.78f, 0.96f, 1.0f, 0.84f);
            glVertex3f(x - sideX, topY, z - sideZ);
            glVertex3f(x + sideX, topY, z + sideZ);
            glColor4f(0.18f, 0.68f, 0.94f, 0.48f);
            glVertex3f(x - sideX, bottomY, z - sideZ);
            glVertex3f(x + sideX, bottomY, z + sideZ);
            glEnd();

            for (int band = 0; band < 3; ++band) {
                const float progress = std::fmod(scene_state::elapsedSeconds * 0.62f +
                    phase + stream * 0.17f + band * 0.33f,
                    1.0f);
                const float bandY = topY - progress * drop;
                const float bandHeight = std::min(0.055f, drop * 0.16f);
                glBegin(GL_TRIANGLE_STRIP);
                glColor4f(0.95f, 1.0f, 1.0f, 0.88f);
                glVertex3f(x - sideX * 0.92f, bandY + bandHeight, z - sideZ * 0.92f);
                glVertex3f(x + sideX * 0.92f, bandY + bandHeight, z + sideZ * 0.92f);
                glColor4f(0.48f, 0.88f, 1.0f, 0.30f);
                glVertex3f(x - sideX * 0.92f, bandY - bandHeight, z - sideZ * 0.92f);
                glVertex3f(x + sideX * 0.92f, bandY - bandHeight, z + sideZ * 0.92f);
                glEnd();
            }
        }
    }

    static void drawWaterImpact(float x, float y, float z, float phase) {
        const float pulse = 0.08f * (0.5f + 0.5f * std::sin(scene_state::elapsedSeconds * 3.0f + phase));
        glColor4f(0.78f, 0.96f, 1.0f, 0.72f);
        glBegin(GL_LINE_LOOP);
        for (int point = 0; point < 24; ++point) {
            const float angle = point * 6.2831853f / 24.0f;
            glVertex3f(x + std::cos(angle) * (0.18f + pulse), y,
                z + std::sin(angle) * (0.12f + pulse * 0.65f));
        }
        glEnd();
    }

    static void drawFountainGarden() {
        for (int vine = 0; vine < 6; ++vine) {
            const float angle = vine * 1.0471976f + 0.2f;
            const float x = std::cos(angle) * 0.82f;
            const float z = std::sin(angle) * 0.82f;
            const float endY = -0.38f - 0.16f * std::sin(scene_state::elapsedSeconds + vine);
            glColor3f(0.18f, 0.48f, 0.20f);
            glBegin(GL_LINES);
            glVertex3f(x, 0.08f, z);
            glVertex3f(x * 1.08f, endY, z * 1.08f);
            glEnd();
            glColor3f(0.62f, 0.76f, 0.28f);
            glPushMatrix();
            glTranslatef(x * 1.04f, endY * 0.55f, z * 1.04f);
            glScalef(0.06f, 0.03f, 0.09f);
            glutSolidSphere(1.0f, 6, 5);
            glPopMatrix();
        }

        for (int flower = 0; flower < 7; ++flower) {
            const float angle = flower * 0.8975979f + 0.3f;
            const float radius = 0.70f + 0.08f * (flower % 3);
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            glColor3f(0.98f, flower % 2 ? 0.72f : 0.88f, 0.94f);
            glPushMatrix();
            glTranslatef(x, 0.12f + 0.02f * std::sin(scene_state::elapsedSeconds + flower), z);
            glutSolidSphere(0.055f, 7, 5);
            glColor3f(1.0f, 0.84f, 0.36f);
            glutSolidSphere(0.022f, 6, 5);
            glPopMatrix();
        }
    }

    static void drawIslandDecorations(float radius, float phase, bool centralIsland) {
        const GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
        const GLboolean blendingWasEnabled = glIsEnabled(GL_BLEND);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        const int flowerCount = centralIsland ? 10 : 3 + static_cast<int>(phase) % 3;
        for (int flower = 0; flower < flowerCount; ++flower) {
            const float angle = flower * 6.2831853f / flowerCount + phase;
            const float distance = radius * (centralIsland ? 0.72f : 0.42f);
            const float x = std::cos(angle) * distance;
            const float z = std::sin(angle) * distance;
            glColor3f(0.24f, 0.58f, 0.23f);
            glBegin(GL_LINES);
            glVertex3f(x, 0.08f, z);
            glVertex3f(x, 0.20f, z);
            glEnd();
            glPushMatrix();
            glTranslatef(x, 0.22f + 0.018f * std::sin(scene_state::elapsedSeconds + flower), z);
            if (flower % 3 == 0) glColor3f(0.56f, 0.92f, 0.92f);
            else if (flower % 3 == 1) glColor3f(0.98f, 0.68f, 0.86f);
            else glColor3f(0.82f, 0.68f, 0.98f);
            glutSolidSphere(0.045f, 6, 5);
            glPopMatrix();
        }
        if (!centralIsland) {
            for (int vine = 0; vine < 2; ++vine) {
                const float angle = phase + vine * 3.1415926f;
                const float x = std::cos(angle) * radius * 0.88f;
                const float z = std::sin(angle) * radius * 0.88f;
                const float endY = -0.46f - 0.12f * std::sin(scene_state::elapsedSeconds + phase + vine);
                glColor3f(0.18f, 0.48f, 0.22f);
                glBegin(GL_LINES);
                glVertex3f(x, 0.08f, z);
                glVertex3f(x * 1.08f, endY, z * 1.08f);
                glEnd();
            }
        }
        if (centralIsland || static_cast<int>(phase * 10.0f) % 2 == 0) {
            glColor3f(0.66f, 0.92f, 1.0f);
            glPushMatrix();
            glTranslatef(radius * 0.45f, 0.28f, -radius * 0.20f);
            glRotatef(scene_state::elapsedSeconds * 25.0f, 0.0f, 1.0f, 0.0f);
            glScalef(0.045f, 0.12f, 0.045f);
            glutSolidSphere(1.0f, 6, 5);
            glPopMatrix();
        }
        const int islandStyle = static_cast<int>(phase * 10.0f) % 4;
        if (!centralIsland && radius >= 1.0f && islandStyle == 1) {
            // Blossom island: several canopy puffs keep the tree visible from afar.
            glColor3f(0.32f, 0.22f, 0.18f);
            glPushMatrix();
            glTranslatef(-0.05f, 0.34f, 0.02f);
            glScalef(0.10f, 0.52f, 0.10f);
            glutSolidCube(1.0f);
            glPopMatrix();
            for (int puff = 0; puff < 4; ++puff) {
                const float angle = puff * 1.5707963f;
                glColor3f(puff % 2 ? 0.86f : 0.98f, 0.62f + puff * 0.03f, 0.84f);
                glPushMatrix();
                glTranslatef(std::cos(angle) * 0.16f, 0.72f + (puff % 2) * 0.10f,
                    std::sin(angle) * 0.16f);
                glScalef(0.28f, 0.20f, 0.28f);
                glutSolidSphere(1.0f, 7, 5);
                glPopMatrix();
            }
        }
        else if (!centralIsland && radius >= 1.0f && islandStyle == 2) {
            // Ruins island: a small broken gateway with a cyan magical capstone.
            for (int side = -1; side <= 1; side += 2) {
                glColor3f(0.43f, 0.42f, 0.50f);
                glPushMatrix();
                glTranslatef(side * 0.24f, 0.30f, 0.0f);
                glScalef(0.10f, 0.54f, 0.10f);
                glutSolidCube(1.0f);
                glPopMatrix();
            }
            glColor3f(0.48f, 0.88f, 0.98f);
            glPushMatrix();
            glTranslatef(0.0f, 0.58f, 0.0f);
            glScalef(0.58f, 0.09f, 0.12f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
        else if (!centralIsland && radius >= 0.8f && islandStyle == 3) {
            // Crystal island: a compact cluster of glowing faceted stones.
            for (int stone = 0; stone < 3; ++stone) {
                const float angle = stone * 2.0943951f;
                glColor3f(0.60f, 0.82f, stone % 2 ? 1.0f : 0.88f);
                glPushMatrix();
                glTranslatef(std::cos(angle) * 0.20f, 0.28f, std::sin(angle) * 0.20f);
                glScalef(0.07f, 0.25f, 0.07f);
                glutSolidSphere(1.0f, 6, 5);
                glPopMatrix();
            }
        }
        if (centralIsland || radius >= 1.0f) {
            const int fireflyCount = centralIsland ? 9 : 5;
            glPointSize(3.5f);
            glBegin(GL_POINTS);
            for (int firefly = 0; firefly < fireflyCount; ++firefly) {
                const float localPhase = phase + firefly * 0.91f;
                const float time = scene_state::elapsedSeconds * 0.70f + localPhase;
                const float homeAngle = firefly * 6.2831853f / fireflyCount;
                const float homeRadius = centralIsland ? radius * 0.62f : radius * 0.36f;
                const float x = std::cos(homeAngle) * homeRadius + 0.06f * std::sin(time);
                const float z = std::sin(homeAngle) * homeRadius + 0.05f * std::cos(time * 1.2f);
                const float alpha = 0.45f + 0.35f * (0.5f + 0.5f * std::sin(time * 2.1f));
                if (firefly % 2 == 0) glColor4f(0.70f, 0.94f, 1.0f, alpha);
                else glColor4f(1.0f, 0.82f, 0.48f, alpha);
                glVertex3f(x, 0.30f + 0.10f * std::sin(time * 1.3f), z);
            }
            glEnd();
            glPointSize(1.0f);
        }
        if (!blendingWasEnabled) glDisable(GL_BLEND);
        if (lightingWasEnabled) glEnable(GL_LIGHTING);
    }

    static void drawCentralFountain() {
        const GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
        const GLboolean blendingWasEnabled = glIsEnabled(GL_BLEND);
        glDisable(GL_LIGHTING);
        drawFountainBasin(0.62f, 0.10f);
        drawFountainBasin(0.40f, 0.43f);
        drawFountainBasin(0.22f, 0.72f);
        drawFountainWater(0.56f, 0.10f, 0.0f);
        drawFountainWater(0.34f, 0.43f, 1.2f);
        drawFountainWater(0.17f, 0.72f, 2.4f);

        const float shimmer = 0.04f * std::sin(scene_state::elapsedSeconds * 2.0f);
        glColor3f(0.62f, 0.90f, 1.0f);
        glPushMatrix();
        glTranslatef(0.0f, 0.90f + shimmer, 0.0f);
        glScalef(0.12f, 0.30f, 0.12f);
        glutSolidSphere(1.0f, 10, 8);
        glPopMatrix();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawWaterCurtains(0.18f, 0.76f, 0.46f, 0.2f);
        drawWaterCurtains(0.37f, 0.46f, 0.13f, 0.8f);
        drawWaterCurtains(0.58f, 0.11f, -1.65f, 0.4f);
        drawWaterImpact(0.58f * std::cos(0.4f), -1.64f, 0.58f * std::sin(0.4f), 0.0f);
        drawWaterImpact(0.58f * std::cos(1.9707963f), -1.64f, 0.58f * std::sin(1.9707963f), 2.0f);
        drawWaterImpact(0.58f * std::cos(3.5415926f), -1.64f, 0.58f * std::sin(3.5415926f), 4.0f);
        drawWaterImpact(0.58f * std::cos(5.1123889f), -1.64f, 0.58f * std::sin(5.1123889f), 6.0f);
        glDisable(GL_BLEND);
        drawFountainGarden();
        if (lightingWasEnabled) glEnable(GL_LIGHTING);
        if (blendingWasEnabled) glEnable(GL_BLEND);
    }

    namespace {

        void updateIslandGrassWind() {
            static float lastUploadTime = -1.0f;
            // Keep the wind animation alive, but update its small dynamic VBO at
            // 20 Hz instead of once per rendered frame.
            if (lastUploadTime >= 0.0f &&
                scene_state::elapsedSeconds - lastUploadTime < 0.05f) return;
            lastUploadTime = scene_state::elapsedSeconds;
            scene_state::animatedIslandGrassVertices = scene_state::islandGrassBaseVertices;
            constexpr std::size_t verticesPerTuft = 126;
            for (std::size_t tuftStart = 0;
                tuftStart < scene_state::animatedIslandGrassVertices.size();
                tuftStart += verticesPerTuft) {
                const std::size_t tuftEnd = std::min(
                    tuftStart + verticesPerTuft,
                    scene_state::animatedIslandGrassVertices.size());
                float rootY = scene_state::animatedIslandGrassVertices[tuftStart].y;
                for (std::size_t i = tuftStart + 1; i < tuftEnd; ++i)
                    rootY = std::min(rootY, scene_state::animatedIslandGrassVertices[i].y);
                const float phase = scene_state::animatedIslandGrassVertices[tuftStart].x * 2.4f +
                    scene_state::animatedIslandGrassVertices[tuftStart].z * 1.7f;
                for (std::size_t i = tuftStart; i < tuftEnd; ++i) {
                    const float bend = std::min(1.0f,
                        std::max(0.0f, (scene_state::animatedIslandGrassVertices[i].y - rootY) / 0.12f));
                    scene_state::animatedIslandGrassVertices[i].x +=
                        std::sin(scene_state::elapsedSeconds * 1.55f + phase) * 0.035f * bend;
                    scene_state::animatedIslandGrassVertices[i].z +=
                        std::cos(scene_state::elapsedSeconds * 1.15f + phase) * 0.014f * bend;
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, scene_state::islandGrassVBO);
            // Use a fresh buffer store for this frame's wind deformation instead of
            // stalling on the GPU's previous read of the same storage.
            glBufferData(GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(scene_state::animatedIslandGrassVertices.size() * sizeof(scene_state::MeshVertex)),
                nullptr, GL_STREAM_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                static_cast<GLsizeiptr>(scene_state::animatedIslandGrassVertices.size() * sizeof(scene_state::MeshVertex)),
                scene_state::animatedIslandGrassVertices.data());
        }

        void drawOrbitingIsland(float baseX, float y, float baseZ, float radius,
            float phase, float angularSpeed) {
            float x = baseX;
            float z = baseZ;
            if (scene_state::islandOrbiting) {
                const float orbitRadius = std::sqrt(baseX * baseX + baseZ * baseZ);
                const float baseAngle = std::atan2(baseZ, baseX);
                const float angle = baseAngle + scene_state::elapsedSeconds * angularSpeed;
                x = std::cos(angle) * orbitRadius;
                z = std::sin(angle) * orbitRadius;
            }
            drawFloatingIsland(x, y, z, radius, phase);
        }

    }

    void drawFloatingIsland(float x, float y, float z, float radius, float phase) {
        const float hover = std::sin(scene_state::elapsedSeconds * 1.15f + phase) * 0.10f;
        const float radialDistance = std::sqrt(x * x + z * z);
        const float radialHeightLift = radialDistance * 0.08f;
        constexpr float scatterFactor = 1.24f;
        rendering::setMaterial({ 0.20f, 0.47f, 0.18f }, { 0.04f, 0.11f, 0.025f }, 3.0f);
        glPushMatrix();
        glTranslatef(x * scatterFactor, y + radialHeightLift + hover, z * scatterFactor);
        glScalef(radius, radius * 0.82f, radius);
        // Keep island grass static for stability; the island's animated hover
        // still moves the complete grass-covered island naturally.
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, scene_state::islandVBO);
        glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
        glNormalPointer(GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glColorPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLES, 0, scene_state::islandVertexCount);
        glBindBuffer(GL_ARRAY_BUFFER, scene_state::islandGrassVBO);
        glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
        glNormalPointer(GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glColorPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLES, 0, scene_state::islandGrassVertexCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_COLOR_MATERIAL);
        const bool centralIsland = std::fabs(x) < 0.01f && std::fabs(z) < 0.01f;
        drawIslandDecorations(radius, phase, centralIsland);
        if (centralIsland) drawCentralFountain();
        glPopMatrix();

    }

    void drawFloatingIslandRings() {
        // A dominant central island with staggered companions and a wider outer
        // ring of varied sizes for a fuller floating-island range.
        drawFloatingIsland(0.0f, 1.25f, 0.0f, 3.40f, 0.2f);
        // Alternating signs make neighboring islands orbit in opposite directions.
        drawOrbitingIsland(-5.20f, 0.65f, -1.55f, 1.05f, 1.1f, 0.22f);
        drawOrbitingIsland(4.70f, 2.55f, -2.10f, 1.18f, 2.0f, -0.18f);
        drawOrbitingIsland(-2.65f, 4.55f, 5.10f, 0.86f, 3.1f, 0.16f);
        drawOrbitingIsland(4.80f, 6.50f, 4.10f, 0.72f, 4.2f, -0.24f);
        drawOrbitingIsland(-7.20f, 2.90f, 2.40f, 0.66f, 5.0f, 0.14f);
        drawOrbitingIsland(7.45f, 4.25f, -5.15f, 0.82f, 5.8f, -0.20f);
        drawOrbitingIsland(-5.10f, 6.80f, -5.60f, 0.58f, 6.6f, 0.19f);
        drawOrbitingIsland(1.80f, 8.15f, 7.20f, 0.70f, 7.4f, -0.15f);
        drawOrbitingIsland(-9.40f, 2.10f, -5.80f, 1.05f, 8.3f, 0.12f);
        drawOrbitingIsland(9.10f, 3.35f, 3.80f, 1.28f, 9.1f, -0.11f);
        drawOrbitingIsland(-8.30f, 5.35f, 7.15f, 0.92f, 10.0f, 0.10f);
        drawOrbitingIsland(6.25f, 7.45f, 8.55f, 1.08f, 10.9f, -0.09f);
        drawOrbitingIsland(-10.20f, 7.80f, -1.35f, 0.76f, 11.8f, 0.13f);
        drawOrbitingIsland(10.15f, 6.15f, -7.60f, 0.96f, 12.7f, -0.12f);
        drawOrbitingIsland(0.20f, 10.10f, -9.25f, 0.84f, 13.6f, 0.10f);
    }

}
