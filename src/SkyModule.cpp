
#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>
#include <vector>

#include "SceneState.h"
#include "SkyModule.h"

namespace sky {

    namespace {

        GLuint skyTexture = 0;

        void drawCloudLayer(const GLfloat boundary[][2], int count, float centerX,
            float centerY, float red, float green, float blue, float alpha,
            float lobeOffset) {
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(red, green, blue, alpha);
            glVertex2f(centerX, centerY);
            for (int index = 0; index <= count; ++index) {
                const int point = index % count;
                const float upperLobe = boundary[point][1] > 0.0f ?
                    std::sin(point * 1.71f + lobeOffset) * 0.035f : 0.0f;
                const float undersideShade = boundary[point][1] < -0.15f ? 0.05f : 0.0f;
                glColor4f(red - undersideShade, green - undersideShade * 0.65f,
                    blue, alpha);
                glVertex2f(boundary[point][0], boundary[point][1] + upperLobe);
            }
            glEnd();
        }

        void drawRoundedCloudPuff(float x, float y, float width, float height,
            float red, float green, float blue) {
            constexpr int segments = 20;
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(red, green, blue, 1.0f);
            glVertex2f(x, y);
            for (int segment = 0; segment <= segments; ++segment) {
                const float angle = static_cast<float>(segment) * 6.2831853f / segments;
                glVertex2f(x + std::cos(angle) * width,
                    y + std::sin(angle) * height);
            }
            glEnd();
        }

        void drawCloudSilhouette(float x, float y, float scale, float widthScale,
            float red, float green, float blue, float lobeOffset) {
            const GLfloat outerBoundary[][2] = {
                { -1.28f, -0.18f },{ -1.12f, -0.34f },{ -0.83f, -0.43f },
                { -0.48f, -0.44f },{ -0.12f, -0.40f },{ 0.25f, -0.38f },
                { 0.62f, -0.32f },{ 0.96f, -0.23f },{ 1.20f, -0.08f },
                { 1.24f, 0.10f },{ 1.08f, 0.27f },{ 0.88f, 0.38f },
                { 0.73f, 0.61f },{ 0.50f, 0.75f },{ 0.28f, 0.73f },
                { 0.08f, 0.65f },{ -0.08f, 0.84f },{ -0.31f, 0.91f },
                { -0.52f, 0.76f },{ -0.66f, 0.57f },{ -0.91f, 0.50f },
                { -1.12f, 0.34f },{ -1.25f, 0.10f }
            };
            const GLfloat bodyBoundary[][2] = {
                { -1.05f, -0.12f },{ -0.76f, -0.27f },{ -0.35f, -0.27f },
                { 0.08f, -0.23f },{ 0.49f, -0.19f },{ 0.84f, -0.10f },
                { 1.00f, 0.09f },{ 0.82f, 0.25f },{ 0.61f, 0.49f },
                { 0.34f, 0.61f },{ 0.08f, 0.51f },{ -0.12f, 0.70f },
                { -0.34f, 0.73f },{ -0.55f, 0.54f },{ -0.78f, 0.40f },
                { -1.00f, 0.18f }
            };
            const GLfloat highlightBoundary[][2] = {
                { -0.61f, 0.24f },{ -0.37f, 0.47f },{ -0.13f, 0.48f },
                { 0.07f, 0.36f },{ 0.28f, 0.53f },{ 0.50f, 0.48f },
                { 0.60f, 0.29f },{ 0.41f, 0.17f },{ 0.16f, 0.19f },
                { -0.06f, 0.07f },{ -0.32f, 0.12f }
            };
            const float aspect = static_cast<float>(scene_state::windowWidth) /
                static_cast<float>(scene_state::windowHeight);

            glPushMatrix();
            glTranslatef(x, y, 0.0f);
            glScalef(scale * widthScale / aspect, scale, 1.0f);
            drawCloudLayer(outerBoundary, sizeof(outerBoundary) / sizeof(outerBoundary[0]),
                0.0f, 0.10f, red * 0.79f, green * 0.78f, 0.93f, 0.90f, lobeOffset);
            drawCloudLayer(bodyBoundary, sizeof(bodyBoundary) / sizeof(bodyBoundary[0]),
                -0.03f, 0.16f, std::fmin(1.0f, red + 0.04f),
                std::fmin(1.0f, green + 0.04f), std::fmin(1.0f, blue + 0.03f),
                0.84f, lobeOffset + 0.45f);
            drawCloudLayer(highlightBoundary,
                sizeof(highlightBoundary) / sizeof(highlightBoundary[0]),
                0.00f, 0.28f, 1.0f, 0.93f, 0.86f, 0.62f, lobeOffset + 0.90f);
            glPopMatrix();
        }

        void drawSkyCloud(float yaw, float elevation, float distance, float scale,
            float widthScale, float red, float green, float blue, float lobeOffset) {
            glPushMatrix();
            glRotatef(yaw, 0.0f, 1.0f, 0.0f);
            glRotatef(elevation, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -distance);
            glScalef(scale * widthScale, scale, 1.0f);

            const float baseRed = red * 0.79f;
            const float baseGreen = green * 0.78f;
            // Opaque matching puffs create one rounded outer cloud contour.
            drawRoundedCloudPuff(-0.95f, -0.04f, 0.42f, 0.30f,
                baseRed, baseGreen, 0.93f);
            drawRoundedCloudPuff(-0.53f, 0.10f, 0.52f, 0.42f,
                baseRed, baseGreen, 0.93f);
            drawRoundedCloudPuff(-0.08f, 0.20f, 0.48f, 0.50f,
                baseRed, baseGreen, 0.93f);
            drawRoundedCloudPuff(0.36f, 0.16f, 0.52f, 0.45f,
                baseRed, baseGreen, 0.93f);
            drawRoundedCloudPuff(0.79f, 0.04f, 0.46f, 0.33f,
                baseRed, baseGreen, 0.93f);
            drawRoundedCloudPuff(0.12f, -0.18f, 0.88f, 0.30f,
                baseRed, baseGreen, 0.93f);

            const GLfloat outerBoundary[][2] = {
                { -1.28f, -0.18f },{ -1.12f, -0.34f },{ -0.83f, -0.43f },
                { -0.48f, -0.44f },{ -0.12f, -0.40f },{ 0.25f, -0.38f },
                { 0.62f, -0.32f },{ 0.96f, -0.23f },{ 1.20f, -0.08f },
                { 1.24f, 0.10f },{ 1.08f, 0.27f },{ 0.88f, 0.38f },
                { 0.73f, 0.61f },{ 0.50f, 0.75f },{ 0.28f, 0.73f },
                { 0.08f, 0.65f },{ -0.08f, 0.84f },{ -0.31f, 0.91f },
                { -0.52f, 0.76f },{ -0.66f, 0.57f },{ -0.91f, 0.50f },
                { -1.12f, 0.34f },{ -1.25f, 0.10f }
            };
            const GLfloat bodyBoundary[][2] = {
                { -1.05f, -0.12f },{ -0.76f, -0.27f },{ -0.35f, -0.27f },
                { 0.08f, -0.23f },{ 0.49f, -0.19f },{ 0.84f, -0.10f },
                { 1.00f, 0.09f },{ 0.82f, 0.25f },{ 0.61f, 0.49f },
                { 0.34f, 0.61f },{ 0.08f, 0.51f },{ -0.12f, 0.70f },
                { -0.34f, 0.73f },{ -0.55f, 0.54f },{ -0.78f, 0.40f },
                { -1.00f, 0.18f }
            };
            const GLfloat highlightBoundary[][2] = {
                { -0.61f, 0.24f },{ -0.37f, 0.47f },{ -0.13f, 0.48f },
                { 0.07f, 0.36f },{ 0.28f, 0.53f },{ 0.50f, 0.48f },
                { 0.60f, 0.29f },{ 0.41f, 0.17f },{ 0.16f, 0.19f },
                { -0.06f, 0.07f },{ -0.32f, 0.12f }
            };
            drawCloudLayer(outerBoundary, sizeof(outerBoundary) / sizeof(outerBoundary[0]),
                0.0f, 0.10f, baseRed, baseGreen, 0.93f, 0.88f, lobeOffset);
            drawCloudLayer(bodyBoundary, sizeof(bodyBoundary) / sizeof(bodyBoundary[0]),
                -0.03f, 0.16f, std::fmin(1.0f, red + 0.04f),
                std::fmin(1.0f, green + 0.04f), std::fmin(1.0f, blue + 0.03f),
                0.82f, lobeOffset + 0.45f);
            drawCloudLayer(highlightBoundary,
                sizeof(highlightBoundary) / sizeof(highlightBoundary[0]),
                0.00f, 0.28f, 1.0f, 0.93f, 0.86f, 0.60f, lobeOffset + 0.90f);
            glPopMatrix();
        }

    }

    void initializeSky() {
        constexpr int width = 256;
        constexpr int height = 128;

        std::vector<unsigned char> pixels(width * height * 4);

        for (int y = 0; y < height; ++y) {
            const float v = static_cast<float>(y) / (height - 1);
            const float horizon = std::fmax(0.0f, 1.0f - v * 2.0f);

            for (int x = 0; x < width; ++x) {
                float red = 104.0f + 136.0f * horizon;
                float green = 122.0f + 72.0f * horizon;
                float blue = 190.0f - 22.0f * horizon;

                const int offset = (y * width + x) * 4;

                pixels[offset + 0] =
                    static_cast<unsigned char>(std::fmin(255.0f, red));
                pixels[offset + 1] =
                    static_cast<unsigned char>(std::fmin(255.0f, green));
                pixels[offset + 2] =
                    static_cast<unsigned char>(std::fmin(255.0f, blue));
                pixels[offset + 3] = 255;
            }
        }

        glGenTextures(1, &skyTexture);
        glBindTexture(GL_TEXTURE_2D, skyTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void drawSky() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisable(GL_BLEND);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, skyTexture);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
        glEnd();

        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        const float nearbyDrift = std::sin(scene_state::elapsedSeconds * 0.012f) * 4.0f;
        const float distantDrift = std::sin(scene_state::elapsedSeconds * 0.007f) * 2.0f;
        GLfloat viewMatrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
        viewMatrix[12] = 0.0f;
        viewMatrix[13] = 0.0f;
        viewMatrix[14] = 0.0f;
        glPushMatrix();
        glLoadMatrixf(viewMatrix);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Large illustrated banks frame the horizon, with smaller puffs above.
        drawSkyCloud(-62.0f + nearbyDrift, 4.0f, 40.0f, 6.6f, 2.05f,
            0.93f, 0.89f, 0.99f, 0.30f);
        drawSkyCloud(48.0f - nearbyDrift * 0.75f, 2.0f, 44.0f, 7.1f, 2.20f,
            1.00f, 0.86f, 0.82f, 1.80f);
        drawSkyCloud(-24.0f + nearbyDrift * 0.40f, 12.0f, 48.0f, 4.2f, 1.52f,
            0.96f, 0.91f, 1.00f, 2.50f);
        drawSkyCloud(14.0f - nearbyDrift * 0.35f, 10.0f, 50.0f, 4.6f, 1.60f,
            0.98f, 0.90f, 0.92f, 0.55f);
        drawSkyCloud(82.0f - nearbyDrift * 0.50f, 14.0f, 52.0f, 3.8f, 1.42f,
            0.98f, 0.90f, 0.87f, 0.80f);
        drawSkyCloud(-82.0f + nearbyDrift * 0.62f, 16.0f, 54.0f, 3.5f, 1.34f,
            0.93f, 0.90f, 0.99f, 1.45f);

        drawSkyCloud(-102.0f + distantDrift, 24.0f, 60.0f, 2.6f, 1.24f,
            0.92f, 0.89f, 0.99f, 3.10f);
        drawSkyCloud(8.0f - distantDrift * 0.70f, 30.0f, 64.0f, 2.3f, 1.12f,
            0.97f, 0.93f, 1.00f, 1.20f);
        drawSkyCloud(116.0f + distantDrift * 0.45f, 27.0f, 67.0f, 2.0f, 1.06f,
            0.96f, 0.90f, 0.96f, 2.90f);
        drawSkyCloud(-48.0f - distantDrift * 0.38f, 39.0f, 70.0f, 1.85f, 1.00f,
            0.95f, 0.92f, 1.00f, 0.60f);
        drawSkyCloud(67.0f + distantDrift * 0.25f, 42.0f, 74.0f, 1.60f, 0.94f,
            1.00f, 0.91f, 0.88f, 1.70f);
        drawSkyCloud(-142.0f - distantDrift * 0.55f, 36.0f, 72.0f, 1.55f, 0.94f,
            0.94f, 0.91f, 1.00f, 0.10f);
        drawSkyCloud(154.0f + distantDrift * 0.30f, 38.0f, 76.0f, 1.30f, 0.88f,
            1.00f, 0.91f, 0.88f, 2.20f);

        glDisable(GL_BLEND);
        glPopMatrix();

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_FOG);
    }

}
