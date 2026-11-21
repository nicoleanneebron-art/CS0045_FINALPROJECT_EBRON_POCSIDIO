#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>
#include <vector>

#include "SkyModule.h"

namespace sky {

namespace {

GLuint skyTexture = 0;

}

void initializeSky() {
    constexpr int width = 256;
    constexpr int height = 128;
    struct CloudBlob { float x; float y; float width; float height; float strength; };
    const CloudBlob cloudBlobs[] = {
        {0.06f, 0.29f, 0.13f, 0.10f, 0.92f}, {0.16f, 0.34f, 0.18f, 0.14f, 0.98f},
        {0.28f, 0.30f, 0.15f, 0.12f, 0.88f}, {0.39f, 0.37f, 0.20f, 0.16f, 0.94f},
        {0.52f, 0.31f, 0.17f, 0.14f, 0.90f}, {0.63f, 0.39f, 0.20f, 0.17f, 0.96f},
        {0.77f, 0.33f, 0.16f, 0.13f, 0.88f}, {0.90f, 0.42f, 0.21f, 0.16f, 0.94f},
        {0.08f, 0.53f, 0.16f, 0.09f, 0.74f}, {0.21f, 0.58f, 0.22f, 0.12f, 0.82f},
        {0.36f, 0.50f, 0.13f, 0.10f, 0.70f}, {0.48f, 0.56f, 0.17f, 0.12f, 0.78f},
        {0.68f, 0.54f, 0.15f, 0.10f, 0.76f}, {0.84f, 0.59f, 0.22f, 0.12f, 0.82f},
        {0.44f, 0.76f, 0.11f, 0.075f, 0.58f}, {0.56f, 0.78f, 0.15f, 0.09f, 0.66f}
    };
    std::vector<unsigned char> pixels(width * height * 4);
    for (int y = 0; y < height; ++y) {
        const float v = static_cast<float>(y) / (height - 1);
        const float horizon = std::fmax(0.0f, 1.0f - v * 2.0f);
        for (int x = 0; x < width; ++x) {
            const float u = static_cast<float>(x) / (width - 1);
            float cloud = 0.0f;
            float cloudCore = 0.0f;
            for (const CloudBlob& blob : cloudBlobs) {
                const float dx = (u - blob.x) / blob.width;
                const float dy = (v - blob.y) / blob.height;
                const float distanceSquared = dx * dx + dy * dy;
                cloud += std::exp(-distanceSquared * 1.75f) * blob.strength;
                cloudCore += std::exp(-distanceSquared * 4.2f) * blob.strength;
            }
            cloud = std::fmin(1.0f, std::fmax(0.0f, (cloud - 0.08f) * 1.55f));
            cloudCore = std::fmin(1.0f, cloudCore);

            float red = 64.0f + 86.0f * horizon;
            float green = 118.0f + 92.0f * horizon;
            float blue = 190.0f + 53.0f * horizon;
            // Bright white-blue clouds with slightly shaded cores.
            red += cloud * 92.0f;
            green += cloud * 88.0f;
            blue += cloud * 64.0f;
            red += cloudCore * 54.0f;
            green += cloudCore * 54.0f;
            blue += cloudCore * 46.0f;
            red -= cloud * 18.0f * (1.0f - horizon);
            green -= cloud * 16.0f * (1.0f - horizon);
            blue -= cloud * 8.0f * (1.0f - horizon);

            const int offset = (y * width + x) * 4;
            pixels[offset + 0] = static_cast<unsigned char>(std::fmin(255.0f, red));
            pixels[offset + 1] = static_cast<unsigned char>(std::fmin(255.0f, green));
            pixels[offset + 2] = static_cast<unsigned char>(std::fmin(255.0f, blue));
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
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FOG);
}

}
