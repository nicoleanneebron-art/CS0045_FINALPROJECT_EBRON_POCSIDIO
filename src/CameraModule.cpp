#include <GL/glew.h>
#include <GL/glut.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "CameraModule.h"
#include "SceneConfig.h"
#include "SceneState.h"

namespace camera {

void applyExplorationCamera() {
    glLoadIdentity();
    const float yaw = scene_state::cameraYaw * scene::DEG_TO_RAD;
    const float pitch = scene_state::cameraPitch * scene::DEG_TO_RAD;
    const float lookX = std::cos(yaw) * std::cos(pitch);
    const float lookY = std::sin(pitch);
    const float lookZ = std::sin(yaw) * std::cos(pitch);
    const float eyeY = scene_state::groundLevelView ? -2.0f : scene_state::cameraY;
    gluLookAt(scene_state::cameraX, eyeY, scene_state::cameraZ,
              scene_state::cameraX + lookX, eyeY + lookY,
              scene_state::cameraZ + lookZ,
              0.0f, 1.0f, 0.0f);
}

void update(float deltaSeconds) {
    static float velocityX = 0.0f;
    static float velocityZ = 0.0f;
    const float yaw = scene_state::cameraYaw * scene::DEG_TO_RAD;
    const float forwardX = std::cos(yaw);
    const float forwardZ = std::sin(yaw);
    const float rightX = -std::sin(yaw);
    const float rightZ = std::cos(yaw);
    const float cameraSpeed = 5.0f;
    float inputX = 0.0f;
    float inputZ = 0.0f;
    if (scene_state::keys['w'] || scene_state::keys['W']) {
        inputX += forwardX;
        inputZ += forwardZ;
    }
    if (scene_state::keys['s'] || scene_state::keys['S']) {
        inputX -= forwardX;
        inputZ -= forwardZ;
    }
    if (scene_state::keys['a'] || scene_state::keys['A']) {
        inputX -= rightX;
        inputZ -= rightZ;
    }
    if (scene_state::keys['d'] || scene_state::keys['D']) {
        inputX += rightX;
        inputZ += rightZ;
    }
    const float inputLength = std::sqrt(inputX * inputX + inputZ * inputZ);
    if (inputLength > 0.0f) {
        inputX = inputX / inputLength * cameraSpeed;
        inputZ = inputZ / inputLength * cameraSpeed;
    }
    const float response = 1.0f - std::exp(-12.0f * deltaSeconds);
    velocityX += (inputX - velocityX) * response;
    velocityZ += (inputZ - velocityZ) * response;
    scene_state::cameraX += velocityX * deltaSeconds;
    scene_state::cameraZ += velocityZ * deltaSeconds;
    scene_state::cameraX = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraX));
    scene_state::cameraZ = std::fmax(-17.0f, std::fmin(17.0f, scene_state::cameraZ));
}

void timer(int) {
    const float now = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    float delta = now - scene_state::lastFrameSeconds;
    scene_state::lastFrameSeconds = now;
    delta = std::fmin(delta, 0.05f);
    update(delta);
    glutPostRedisplay();
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
}

void keyboardDown(unsigned char key, int, int) {
    if (key == 27) std::exit(0);
    scene_state::keys[key] = true;
    if (key == 'g' || key == 'G') {
        scene_state::groundLevelView = !scene_state::groundLevelView;
    }
}

void keyboardUp(unsigned char key, int, int) {
    scene_state::keys[key] = false;
}

void specialDown(int key, int, int) {
    if (key == GLUT_KEY_LEFT) scene_state::keys['a'] = true;
    if (key == GLUT_KEY_RIGHT) scene_state::keys['d'] = true;
    if (key == GLUT_KEY_UP) scene_state::keys['w'] = true;
    if (key == GLUT_KEY_DOWN) scene_state::keys['s'] = true;
}

void mouseButton(int button, int state, int, int) {
    if (state == GLUT_DOWN && (button == 3 || button == 4)) {
        scene_state::cameraFieldOfView += button == 3 ? -3.0f : 3.0f;
        scene_state::cameraFieldOfView = std::fmax(
            30.0f, std::fmin(85.0f, scene_state::cameraFieldOfView));
        reshape(scene_state::windowWidth, scene_state::windowHeight);
    }
}

void mouseLook(int x, int y) {
    if (!scene_state::mouseInsideWindow) return;
    if (!scene_state::mouseLookInitialized) {
        scene_state::lastMouseX = x;
        scene_state::lastMouseY = y;
        scene_state::mouseLookInitialized = true;
        return;
    }
    const int deltaX = x - scene_state::lastMouseX;
    const int deltaY = y - scene_state::lastMouseY;
    scene_state::lastMouseX = x;
    scene_state::lastMouseY = y;
    constexpr float sensitivity = 0.16f;
    scene_state::cameraYaw += static_cast<float>(deltaX) * sensitivity;
    scene_state::cameraPitch -= static_cast<float>(deltaY) * sensitivity;
    scene_state::cameraPitch = std::fmax(
        -85.0f, std::fmin(85.0f, scene_state::cameraPitch));
}

void mouseEntry(int state) {
    scene_state::mouseInsideWindow = state == GLUT_ENTERED;
    scene_state::mouseLookInitialized = false;
}

}
