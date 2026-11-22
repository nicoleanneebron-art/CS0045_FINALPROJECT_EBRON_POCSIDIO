#include <GL/glew.h>
#include <GL/glut.h>

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
    } else {
        glutReshapeWindow(windowedWidth, windowedHeight);
        glutPositionWindow(windowedX, windowedY);
    }
    isFullScreen = !isFullScreen;
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
        const float eyeY = butterflyY + 2.75f;
        const float eyeZ = scene_state::cameraZ - horizontalZ * 4.5f;
        gluLookAt(eyeX, eyeY, eyeZ,
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
    const float cameraSpeed = 5.0f;
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
    scene_state::cameraX += velocityX * deltaSeconds;
    scene_state::cameraY += velocityY * deltaSeconds;
    scene_state::cameraZ += velocityZ * deltaSeconds;
    scene_state::cameraX = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraX));
    scene_state::cameraY = std::fmax(0.0f, std::fmin(20.0f, scene_state::cameraY));
    scene_state::cameraZ = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraZ));
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
