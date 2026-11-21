#include <GL/glew.h>
#include <GL/glut.h>

#include <algorithm>
#include <cmath>

#include "FloatingIslandsModule.h"
#include "RenderingModule.h"
#include "SceneState.h"

namespace floating_islands {

namespace {

void updateIslandGrassWind() {
    static float lastUploadTime = -1.0f;
    if (lastUploadTime == scene_state::elapsedSeconds) return;
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
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(scene_state::animatedIslandGrassVertices.size() * sizeof(scene_state::MeshVertex)),
                    scene_state::animatedIslandGrassVertices.data());
}

}

void drawFloatingIsland(float x, float y, float z, float radius, float phase) {
    const float hover = std::sin(scene_state::elapsedSeconds * 1.15f + phase) * 0.10f;
    const float radialDistance = std::sqrt(x * x + z * z);
    const float radialHeightLift = radialDistance * 0.08f;
    constexpr float scatterFactor = 1.24f;
    rendering::setMaterial({0.20f, 0.47f, 0.18f}, {0.04f, 0.11f, 0.025f}, 3.0f);
    glPushMatrix();
    glTranslatef(x * scatterFactor, y + radialHeightLift + hover, z * scatterFactor);
    glScalef(radius, radius * 0.82f, radius);
    updateIslandGrassWind();
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
    glPopMatrix();

}

void drawFloatingIslandRings() {
    // One dominant central island with four smaller, staggered companions.
    drawFloatingIsland(0.0f, 1.25f, 0.0f, 3.40f, 0.2f);
    drawFloatingIsland(-5.20f, 0.65f, -1.55f, 1.05f, 1.1f);
    drawFloatingIsland(4.70f, 2.55f, -2.10f, 1.18f, 2.0f);
    drawFloatingIsland(-2.65f, 4.55f, 5.10f, 0.86f, 3.1f);
    drawFloatingIsland(4.80f, 6.50f, 4.10f, 0.72f, 4.2f);
    drawFloatingIsland(-7.20f, 2.90f, 2.40f, 0.66f, 5.0f);
    drawFloatingIsland(7.45f, 4.25f, -5.15f, 0.82f, 5.8f);
    drawFloatingIsland(-5.10f, 6.80f, -5.60f, 0.58f, 6.6f);
    drawFloatingIsland(1.80f, 8.15f, 7.20f, 0.70f, 7.4f);
}

}
