#include <GL/glew.h>
#include <GL/glut.h>
#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "CameraModule.h"
#include "SceneConfig.h"
#include "SceneState.h"

namespace {

    bool isFullScreen = false;
    int windowedWidth = 0;
    int windowedHeight = 0;
    int windowedX = 0;
    int windowedY = 0;

    void toggleFullScreen() {
        if (!isFullScreen) {
            windowedWidth = glutGet(GLUT_WINDOW_WIDTH);
            windowedHeight = glutGet(GLUT_WINDOW_HEIGHT);
            windowedX = glutGet(GLUT_WINDOW_X);
            windowedY = glutGet(GLUT_WINDOW_Y);
            glutFullScreen();
        }
        else {
            glutReshapeWindow(windowedWidth, windowedHeight);
            glutPositionWindow(windowedX, windowedY);
        }
        isFullScreen = !isFullScreen;
    }

    void getIslandCollision(int index, float& centerX, float& centerY,
        float& centerZ, float& radius) {
        const float islands[][6] = {
            { 0.0f, 1.25f, 0.0f, 3.40f, 0.2f, 0.0f },
            { -5.20f, 0.65f, -1.55f, 1.05f, 1.1f, 0.22f },
            { 4.70f, 2.55f, -2.10f, 1.18f, 2.0f, -0.18f },
            { -2.65f, 4.55f, 5.10f, 0.86f, 3.1f, 0.16f },
            { 4.80f, 6.50f, 4.10f, 0.72f, 4.2f, -0.24f },
            { -7.20f, 2.90f, 2.40f, 0.66f, 5.0f, 0.14f },
            { 7.45f, 4.25f, -5.15f, 0.82f, 5.8f, -0.20f },
            { -5.10f, 6.80f, -5.60f, 0.58f, 6.6f, 0.19f },
            { 1.80f, 8.15f, 7.20f, 0.70f, 7.4f, -0.15f },
            { -9.40f, 2.10f, -5.80f, 1.05f, 8.3f, 0.12f },
            { 9.10f, 3.35f, 3.80f, 1.28f, 9.1f, -0.11f },
            { -8.30f, 5.35f, 7.15f, 0.92f, 10.0f, 0.10f },
            { 6.25f, 7.45f, 8.55f, 1.08f, 10.9f, -0.09f },
            { -10.20f, 7.80f, -1.35f, 0.76f, 11.8f, 0.13f },
            { 10.15f, 6.15f, -7.60f, 0.96f, 12.7f, -0.12f },
            { 0.20f, 10.10f, -9.25f, 0.84f, 13.6f, 0.10f }
        };
        float x = islands[index][0];
        const float y = islands[index][1];
        float z = islands[index][2];
        radius = islands[index][3];
        if (index > 0 && scene_state::islandOrbiting) {
            const float orbitRadius = std::sqrt(x * x + z * z);
            const float baseAngle = std::atan2(z, x);
            const float angle = baseAngle + scene_state::elapsedSeconds * islands[index][5];
            x = std::cos(angle) * orbitRadius;
            z = std::sin(angle) * orbitRadius;
        }
        const float hover = std::sin(scene_state::elapsedSeconds * 1.15f +
            islands[index][4]) * 0.10f;
        const float radialLift = std::sqrt(x * x + z * z) * 0.08f;
        centerX = x * 1.24f * scene::mapScale;
        centerY = (y + radialLift + hover - radius * 0.35f) * scene::mapScale;
        centerZ = z * 1.24f * scene::mapScale;
        radius = radius * 0.78f * scene::mapScale;
    }

    void pushOutsideSphere(float& x, float& y, float& z, float centerX,
        float centerY, float centerZ, float radius) {
        const float dx = x - centerX;
        const float dy = y - centerY;
        const float dz = z - centerZ;
        const float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (distance >= radius) return;
        if (distance < 0.001f) {
            y = centerY + radius;
            return;
        }
        const float correction = radius - distance;
        x += dx / distance * correction;
        y += dy / distance * correction;
        z += dz / distance * correction;
    }

    void resolveButterflyCollision(float& x, float& y, float& z) {
        constexpr float butterflyRadius = 0.45f;
        float bodyY = y - 1.50f;
        for (int index = 0; index < 16; ++index) {
            float islandX = 0.0f;
            float islandY = 0.0f;
            float islandZ = 0.0f;
            float islandRadius = 0.0f;
            getIslandCollision(index, islandX, islandY, islandZ, islandRadius);
            pushOutsideSphere(x, bodyY, z, islandX, islandY, islandZ,
                islandRadius + butterflyRadius);
        }
        const float centralHover = std::sin(scene_state::elapsedSeconds * 1.15f + 0.2f) * 0.10f;
        const float treeBaseY = (1.25f + centralHover) * scene::mapScale;
        const float treeTopY = treeBaseY + 12.0f;
        const float treeRadius = 0.65f;
        const float treeDistance = std::sqrt(x * x + z * z);
        if (bodyY >= treeBaseY && bodyY <= treeTopY &&
            treeDistance < treeRadius + butterflyRadius) {
            const float safeDistance = treeRadius + butterflyRadius;
            if (treeDistance < 0.001f) x = safeDistance;
            else {
                x = x / treeDistance * safeDistance;
                z = z / treeDistance * safeDistance;
            }
        }
        y = bodyY + 1.50f;
    }

    void adjustChaseCamera(float startX, float startY, float startZ,
        float& eyeX, float& eyeY, float& eyeZ) {
        const float directionX = eyeX - startX;
        const float directionY = eyeY - startY;
        const float directionZ = eyeZ - startZ;
        const float length = std::sqrt(directionX * directionX + directionY * directionY +
            directionZ * directionZ);
        if (length < 0.001f) return;
        float safeLength = length;
        for (int index = 0; index < 16; ++index) {
            float islandX = 0.0f;
            float islandY = 0.0f;
            float islandZ = 0.0f;
            float islandRadius = 0.0f;
            getIslandCollision(index, islandX, islandY, islandZ, islandRadius);
            const float relativeX = islandX - startX;
            const float relativeY = islandY - startY;
            const float relativeZ = islandZ - startZ;
            const float projection = (relativeX * directionX + relativeY * directionY +
                relativeZ * directionZ) / (length * length);
            if (projection <= 0.0f || projection >= 1.0f) continue;
            const float closestX = startX + directionX * projection;
            const float closestY = startY + directionY * projection;
            const float closestZ = startZ + directionZ * projection;
            const float dx = islandX - closestX;
            const float dy = islandY - closestY;
            const float dz = islandZ - closestZ;
            const float clearance = islandRadius + 0.30f;
            if (dx * dx + dy * dy + dz * dz < clearance * clearance) {
                safeLength = std::fmin(safeLength,
                    std::fmax(1.20f, projection * length - clearance));
            }
        }
        const float scale = safeLength / length;
        eyeX = startX + directionX * scale;
        eyeY = startY + directionY * scale;
        eyeZ = startZ + directionZ * scale;
    }

}

namespace camera {

    static bool leftArrowDown = false;
    static bool rightArrowDown = false;
    static bool upArrowDown = false;
    static bool downArrowDown = false;
    static bool returningToGround = false;
    constexpr float lowHoverHeight = 1.50f;
    constexpr float returnDescentSpeed = 2.50f;

    void applyExplorationCamera() {
        glLoadIdentity();
        const float yaw = scene_state::cameraYaw * scene::DEG_TO_RAD;
        const float pitch = scene_state::cameraPitch * scene::DEG_TO_RAD;
        const float lookX = std::cos(yaw) * std::cos(pitch);
        const float lookY = std::sin(pitch);
        const float lookZ = std::sin(yaw) * std::cos(pitch);

        if (scene_state::thirdPersonView) {
            const float butterflyY = scene_state::cameraY - 1.50f;
            const float horizontalX = std::cos(yaw);
            const float horizontalZ = std::sin(yaw);
            const float eyeX = scene_state::cameraX - horizontalX * 4.5f;
            float adjustedEyeX = eyeX;
            float adjustedEyeY = butterflyY + 2.75f;
            float adjustedEyeZ = scene_state::cameraZ - horizontalZ * 4.5f;
            adjustChaseCamera(scene_state::cameraX, butterflyY + 0.20f,
                scene_state::cameraZ, adjustedEyeX, adjustedEyeY,
                adjustedEyeZ);
            gluLookAt(adjustedEyeX, adjustedEyeY, adjustedEyeZ,
                scene_state::cameraX + lookX * 1.50f,
                butterflyY + 0.15f + lookY * 1.00f,
                scene_state::cameraZ + lookZ * 1.50f,
                0.0f, 1.0f, 0.0f);
            return;
        }

        gluLookAt(scene_state::cameraX, scene_state::cameraY, scene_state::cameraZ,
            scene_state::cameraX + lookX, scene_state::cameraY + lookY,
            scene_state::cameraZ + lookZ,
            0.0f, 1.0f, 0.0f);
    }

    void update(float deltaSeconds) {
        static float velocityX = 0.0f;
        static float velocityY = 0.0f;
        static float velocityZ = 0.0f;
        const float yaw = scene_state::cameraYaw * scene::DEG_TO_RAD;
        const float pitch = scene_state::cameraPitch * scene::DEG_TO_RAD;
        const float forwardX = std::cos(yaw) * std::cos(pitch);
        const float forwardY = std::sin(pitch);
        const float forwardZ = std::sin(yaw) * std::cos(pitch);
        const float rightX = -std::sin(yaw);
        const float rightZ = std::cos(yaw);
        const bool boostActive = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const float cameraSpeed = boostActive ? 10.0f : 5.0f;
        float inputX = 0.0f;
        float inputY = 0.0f;
        float inputZ = 0.0f;
        const bool movingForward = scene_state::keys['w'] || scene_state::keys['W'] ||
            upArrowDown;
        const bool movingBackward = scene_state::keys['s'] || scene_state::keys['S'] ||
            downArrowDown;
        const bool movingLeft = scene_state::keys['a'] || scene_state::keys['A'] ||
            leftArrowDown;
        const bool movingRight = scene_state::keys['d'] || scene_state::keys['D'] ||
            rightArrowDown;
        const bool manualMovement = movingForward || movingBackward || movingLeft ||
            movingRight;
        if (manualMovement) returningToGround = false;

        if (movingForward) {
            inputX += forwardX;
            inputY += forwardY;
            inputZ += forwardZ;
        }
        if (movingBackward) {
            inputX -= forwardX;
            inputY -= forwardY;
            inputZ -= forwardZ;
        }
        if (movingLeft) {
            inputX -= rightX;
            inputZ -= rightZ;
        }
        if (movingRight) {
            inputX += rightX;
            inputZ += rightZ;
        }
        if (returningToGround) {
            inputY = -returnDescentSpeed;
            if (velocityY > 0.0f) velocityY = 0.0f;
        }
        const float inputLength = std::sqrt(inputX * inputX + inputY * inputY +
            inputZ * inputZ);
        if (inputLength > 0.0f) {
            inputX = inputX / inputLength * cameraSpeed;
            inputY = inputY / inputLength * cameraSpeed;
            inputZ = inputZ / inputLength * cameraSpeed;
        }
        // Respond quickly to keyboard changes while remaining frame-rate
        // independent. The old response made movement feel sluggish at 60 Hz.
        const float response = 1.0f - std::exp(-24.0f * deltaSeconds);
        velocityX += (inputX - velocityX) * response;
        velocityY += (inputY - velocityY) * response;
        velocityZ += (inputZ - velocityZ) * response;
        const float previousX = scene_state::cameraX;
        const float previousY = scene_state::cameraY;
        const float previousZ = scene_state::cameraZ;
        scene_state::cameraX += velocityX * deltaSeconds;
        scene_state::cameraY += velocityY * deltaSeconds;
        scene_state::cameraZ += velocityZ * deltaSeconds;
        scene_state::cameraX = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraX));
        scene_state::cameraY = std::fmax(0.0f, std::fmin(20.0f, scene_state::cameraY));
        scene_state::cameraZ = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraZ));
        resolveButterflyCollision(scene_state::cameraX, scene_state::cameraY,
            scene_state::cameraZ);
        scene_state::cameraX = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraX));
        scene_state::cameraY = std::fmax(0.0f, std::fmin(20.0f, scene_state::cameraY));
        scene_state::cameraZ = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraZ));
        if (deltaSeconds > 0.0f) {
            velocityX = (scene_state::cameraX - previousX) / deltaSeconds;
            velocityY = (scene_state::cameraY - previousY) / deltaSeconds;
            velocityZ = (scene_state::cameraZ - previousZ) / deltaSeconds;
        }
        // Stop small residual movement when no keys are held.
        if (!manualMovement && !returningToGround) {
            const float stopThreshold = 0.05f;

            if (std::fabs(velocityX) < stopThreshold) velocityX = 0.0f;
            if (std::fabs(velocityY) < stopThreshold) velocityY = 0.0f;
            if (std::fabs(velocityZ) < stopThreshold) velocityZ = 0.0f;
        }


        if (returningToGround && scene_state::cameraY <= lowHoverHeight) {
            scene_state::cameraY = lowHoverHeight;
            velocityY = 0.0f;
            returningToGround = false;
        }
    }

    void timer(int) {
        const float now = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
        float delta = now - scene_state::lastFrameSeconds;
        scene_state::lastFrameSeconds = now;
        delta = std::fmin(delta, 0.05f);
        update(delta);
        glutPostRedisplay();
        // Keep redraw scheduling at 60 Hz so slow frames cannot backlog timer
        // redisplays. The faster acceleration below still makes input responsive.
        glutTimerFunc(16, timer, 0);
    }

    void reshape(int width, int height) {
        scene_state::windowWidth = width;
        scene_state::windowHeight = height == 0 ? 1 : height;
        glViewport(0, 0, scene_state::windowWidth, scene_state::windowHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(scene_state::cameraFieldOfView,
            static_cast<double>(scene_state::windowWidth) /
            scene_state::windowHeight,
            0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);

        if (scene_state::mouseInsideWindow) {
            scene_state::lastMouseX = scene_state::windowWidth / 2;
            scene_state::lastMouseY = scene_state::windowHeight / 2;
            scene_state::mouseLookInitialized = true;
            glutWarpPointer(scene_state::lastMouseX, scene_state::lastMouseY);
        }
    }

    void keyboardDown(unsigned char key, int, int) {
        if (key == 27) std::exit(0);
        const bool wasDown = scene_state::keys[key];
        scene_state::keys[key] = true;
        if ((key == 'f' || key == 'F') && !wasDown) {
            toggleFullScreen();
        }
        if ((key == 'p' || key == 'P') && !wasDown) {
            scene_state::thirdPersonView = !scene_state::thirdPersonView;
        }
        if ((key == 'g' || key == 'G') && !wasDown) {
            returningToGround = scene_state::cameraY > lowHoverHeight;
        }
    }

    void keyboardUp(unsigned char key, int, int) {
        scene_state::keys[key] = false;
    }

    void specialDown(int key, int, int) {
        if (key == GLUT_KEY_LEFT) leftArrowDown = true;
        if (key == GLUT_KEY_RIGHT) rightArrowDown = true;
        if (key == GLUT_KEY_UP) upArrowDown = true;
        if (key == GLUT_KEY_DOWN) downArrowDown = true;
    }

    void specialUp(int key, int, int) {
        if (key == GLUT_KEY_LEFT) leftArrowDown = false;
        if (key == GLUT_KEY_RIGHT) rightArrowDown = false;
        if (key == GLUT_KEY_UP) upArrowDown = false;
        if (key == GLUT_KEY_DOWN) downArrowDown = false;
    }

    bool isForwardInputActive() {
        return scene_state::keys['w'] || scene_state::keys['W'] || upArrowDown;
    }

    bool isReturningToGround() {
        return returningToGround;
    }

    void mouseButton(int button, int state, int, int) {
        if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
            scene_state::islandOrbiting = !scene_state::islandOrbiting;
            return;
        }
        if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
            scene_state::crystalAngle += 30.0f;
            if (scene_state::crystalAngle >= 360.0f) {
                scene_state::crystalAngle -= 360.0f;
            }
            glutPostRedisplay();
            return;
        }
        if (state == GLUT_DOWN && (button == 3 || button == 4)) {
            scene_state::butterflyScale += button == 3 ? 0.08f : -0.08f;
            scene_state::butterflyScale = std::fmax(
                0.50f, std::fmin(1.20f, scene_state::butterflyScale));
        }
    }

    void mouseLook(int x, int y) {
        if (!scene_state::mouseInsideWindow) return;

        const int centerX = scene_state::windowWidth / 2;
        const int centerY = scene_state::windowHeight / 2;

        if (!scene_state::mouseLookInitialized) {
            scene_state::lastMouseX = centerX;
            scene_state::lastMouseY = centerY;
            scene_state::mouseLookInitialized = true;
            glutWarpPointer(centerX, centerY);
            return;
        }

        // glutWarpPointer produces a passive-motion event at the center.
        if (x == centerX && y == centerY) return;

        const int deltaX = x - centerX;
        const int deltaY = y - centerY;
        constexpr float sensitivity = 0.16f;
        scene_state::cameraYaw += static_cast<float>(deltaX) * sensitivity;
        scene_state::cameraPitch -= static_cast<float>(deltaY) * sensitivity;
        scene_state::cameraPitch = std::fmax(
            -85.0f, std::fmin(85.0f, scene_state::cameraPitch));

        scene_state::lastMouseX = centerX;
        scene_state::lastMouseY = centerY;
        glutWarpPointer(centerX, centerY);
    }

    void mouseEntry(int state) {
        scene_state::mouseInsideWindow = state == GLUT_ENTERED;
        scene_state::mouseLookInitialized = false;

        if (scene_state::mouseInsideWindow) {
            glutWarpPointer(scene_state::windowWidth / 2,
                scene_state::windowHeight / 2);
        }
    }

}
