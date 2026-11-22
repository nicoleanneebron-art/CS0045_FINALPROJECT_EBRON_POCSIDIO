#pragma once

namespace camera {

void applyExplorationCamera();
void update(float deltaSeconds);
void timer(int value);
void reshape(int width, int height);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);
bool isForwardInputActive();
bool isReturningToGround();
void mouseButton(int button, int state, int x, int y);
void mouseLook(int x, int y);
void mouseEntry(int state);

}
