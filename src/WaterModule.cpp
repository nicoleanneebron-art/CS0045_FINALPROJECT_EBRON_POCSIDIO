#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>
#include <vector>

#include "RenderingModule.h"
#include "SceneState.h"
#include "WaterModule.h"

namespace water {

float waterSurfaceHeight(float x, float z) {
    const float time = scene_state::elapsedSeconds;
    return -3.18f +
           0.045f * std::sin(x * 0.55f + time * 1.6f) +
           0.035f * std::cos(z * 0.45f - time * 1.2f) +
           0.020f * std::sin((x + z) * 0.30f + time * 1.9f);
}

void drawWater() {
    rendering::setMaterial({0.58f, 0.88f, 0.98f}, {0.10f, 0.24f, 0.32f}, 24.0f);
    const GLfloat waterSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
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
}

}
