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

void drawGrass() {
    const bool bufferSizeChanged =
        scene_state::animatedGrassVertices.size() != scene_state::grassBaseVertices.size();
    if (bufferSizeChanged) {
        scene_state::animatedGrassVertices.resize(scene_state::grassBaseVertices.size());
    }
    const bool updateAnimation = bufferSizeChanged ||
                                 ((scene_state::grassAnimationFrame++ & 1u) == 0u);
    if (updateAnimation) {
        scene_state::animatedGrassVertices = scene_state::grassBaseVertices;
    }
    std::vector<scene_state::MeshVertex>& animatedGrass = scene_state::animatedGrassVertices;
    constexpr std::size_t verticesPerTuft = 90;
    if (updateAnimation) for (std::size_t tuftStart = 0; tuftStart < animatedGrass.size(); tuftStart += verticesPerTuft) {
        const std::size_t tuftEnd = std::min(tuftStart + verticesPerTuft, animatedGrass.size());
        float rootY = animatedGrass[tuftStart].y;
        for (std::size_t i = tuftStart + 1; i < tuftEnd; ++i)
            rootY = std::min(rootY, animatedGrass[i].y);
        const float phase = animatedGrass[tuftStart].x * 0.42f + animatedGrass[tuftStart].z * 0.31f;
        for (std::size_t i = tuftStart; i < tuftEnd; ++i) {
            const float height = std::max(0.0f, animatedGrass[i].y - rootY);
            const float bend = std::min(1.0f, height / 0.30f);
            animatedGrass[i].x += std::sin(scene_state::elapsedSeconds * 1.55f + phase) * 0.16f * bend;
            animatedGrass[i].z += std::cos(scene_state::elapsedSeconds * 1.15f + phase * 1.3f) * 0.055f * bend;
        }
    }
    if (updateAnimation) {
        glBindBuffer(GL_ARRAY_BUFFER, scene_state::grassVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        static_cast<GLsizeiptr>(animatedGrass.size() * sizeof(scene_state::MeshVertex)),
                        animatedGrass.data());
    }
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
