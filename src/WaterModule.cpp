#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>
#include <vector>

#include "RenderingModule.h"
#include "SceneConfig.h"
#include "SceneState.h"
#include "WaterModule.h"

namespace water {

    float waterSurfaceHeight(float x, float z) {
        const float time = scene_state::elapsedSeconds;
        return scene::waterLevel +
            0.045f * std::sin(x * 0.55f + time * 1.6f) +
            0.035f * std::cos(z * 0.45f - time * 1.2f) +
            0.020f * std::sin((x + z) * 0.30f + time * 1.9f);
    }

    static void drawLilyPad(float x, float z, float size, float phase) {
        const float y = waterSurfaceHeight(x, z) + 0.025f +
            0.008f * std::sin(scene_state::elapsedSeconds * 1.3f + phase);
        const float rotation = 0.08f * std::sin(scene_state::elapsedSeconds * 0.7f + phase);
        const float shade = 0.04f * static_cast<float>(static_cast<int>(phase) % 3);
        glColor4f(0.20f + shade, 0.50f + shade, 0.25f + shade * 0.4f, 0.92f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(x, y, z);
        for (int point = 0; point <= 14; ++point) {
            const float rawAngle = point * 6.2831853f / 14.0f;
            const float angle = rawAngle + rotation;
            const float gap = std::fabs(rawAngle - 1.5707963f) < 0.28f ? 0.60f : 1.0f;
            glVertex3f(x + std::cos(angle) * size * gap, y,
                z + std::sin(angle) * size * gap);
        }
        glEnd();
        if (static_cast<int>(phase) % 2 == 0) {
            glPushMatrix();
            glTranslatef(x, y + 0.045f, z);
            const int petals = size > 0.34f ? 7 : 5;
            for (int petal = 0; petal < petals; ++petal) {
                const float angle = petal * 6.2831853f / petals + rotation;
                if (static_cast<int>(phase) % 4 == 0) glColor3f(0.98f, 0.70f, 0.86f);
                else if (static_cast<int>(phase) % 4 == 2) glColor3f(0.82f, 0.70f, 0.98f);
                else glColor3f(0.98f, 0.92f, 0.94f);
                glPushMatrix();
                glTranslatef(std::cos(angle) * size * 0.31f, 0.0f, std::sin(angle) * size * 0.31f);
                glutSolidSphere(size * (size > 0.34f ? 0.25f : 0.22f), 6, 5);
                glPopMatrix();
            }
            glColor3f(1.0f, 0.84f, 0.40f);
            glutSolidSphere(size * 0.16f, 6, 5);
            glPopMatrix();
        }
    }

    static void drawLilyRipple(float x, float z, float phase) {
        const float y = waterSurfaceHeight(x, z) + 0.032f;
        const float pulse = 0.10f + 0.07f * (0.5f + 0.5f *
            std::sin(scene_state::elapsedSeconds * 1.7f + phase));
        glColor4f(0.72f, 0.94f, 1.0f, 0.42f);
        glBegin(GL_LINE_LOOP);
        for (int point = 0; point < 20; ++point) {
            const float angle = point * 6.2831853f / 20.0f;
            glVertex3f(x + std::cos(angle) * pulse, y,
                z + std::sin(angle) * pulse * 0.70f);
        }
        glEnd();
    }

    static void drawAquaticReeds(float x, float z, float phase) {
        const float y = waterSurfaceHeight(x, z);
        for (int reed = 0; reed < 5; ++reed) {
            const float angle = reed * 1.2566371f;
            const float sway = 0.05f * std::sin(scene_state::elapsedSeconds + phase + reed);
            glColor3f(0.22f, 0.50f, 0.27f);
            glBegin(GL_LINES);
            glVertex3f(x + std::cos(angle) * 0.10f, y, z + std::sin(angle) * 0.10f);
            glVertex3f(x + std::cos(angle) * 0.10f + sway, y + 0.32f,
                z + std::sin(angle) * 0.10f);
            glEnd();
        }
    }

    static void drawLakeDetails() {
        const GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        const float pads[][3] = {
            // Small shoreline pads.
            { -7.8f, 2.0f, 0.14f },{ -7.2f, 3.1f, 0.16f },{ -6.6f, 4.0f, 0.15f },
            { 6.9f, 3.7f, 0.15f },{ 7.8f, -2.3f, 0.16f },{ 4.9f, -6.8f, 0.14f },
            // Medium pads form quieter shoreline groups.
            { -6.8f, 3.8f, 0.27f },{ -5.9f, -5.0f, 0.25f },{ 6.5f, 3.4f, 0.28f },
            { 7.4f, -2.9f, 0.26f },{ -2.5f, -7.7f, 0.24f },{ 2.8f, 7.0f, 0.25f },
            // Large pads remain at the lake edge and keep the center open.
            { -8.6f, 4.9f, 0.46f },{ 8.3f, -3.7f, 0.42f },{ -4.0f, -7.5f, 0.40f },
            { 4.1f, 7.4f, 0.44f }
        };
        for (int pad = 0; pad < 16; ++pad) {
            drawLilyPad(pads[pad][0], pads[pad][1], pads[pad][2], static_cast<float>(pad));
            if (pads[pad][2] > 0.38f) drawLilyRipple(pads[pad][0], pads[pad][1], static_cast<float>(pad));
        }
        drawAquaticReeds(-8.9f, 3.9f, 0.3f);
        drawAquaticReeds(8.2f, -4.2f, 1.6f);
        drawAquaticReeds(-3.7f, -7.4f, 2.9f);

        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for (int sparkle = 0; sparkle < 12; ++sparkle) {
            const float phase = sparkle * 0.83f;
            const float x = -7.0f + (sparkle % 6) * 2.7f;
            const float z = -5.5f + (sparkle / 6) * 10.0f;
            const float pulse = 0.5f + 0.5f * std::sin(scene_state::elapsedSeconds * 2.0f + phase);
            glColor4f(0.62f, 0.94f, 1.0f, 0.35f + pulse * 0.45f);
            glVertex3f(x, waterSurfaceHeight(x, z) + 0.04f, z);
        }
        glEnd();
        glPointSize(1.0f);
        glDisable(GL_BLEND);
        if (lightingWasEnabled) glEnable(GL_LIGHTING);
    }

    void drawWater() {
        rendering::setMaterial({ 0.58f, 0.88f, 0.98f }, { 0.10f, 0.24f, 0.32f }, 24.0f);
        const GLfloat waterSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, waterSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96.0f);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, scene_state::waterTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.78f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, scene_state::waterVBO);
        glVertexPointer(3, GL_FLOAT, sizeof(scene_state::WaterVertex), reinterpret_cast<void*>(0));
        glNormalPointer(GL_FLOAT, sizeof(scene_state::WaterVertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(scene_state::WaterVertex), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLES, 0, scene_state::waterVertexCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        drawLakeDetails();
    }

}
