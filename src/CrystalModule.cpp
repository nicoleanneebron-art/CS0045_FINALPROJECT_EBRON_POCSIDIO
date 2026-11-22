#include <GL/glew.h>
#include <GL/glut.h>

#include "CrystalModule.h"
#include "SceneState.h"

namespace crystal {

void drawCrystal() {
    const GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(-2.4f, 1.70f, -1.6f);
    glRotatef(scene_state::crystalAngle, 0.0f, 1.0f, 0.0f);

    const GLfloat upperTip[] = {0.0f, 1.15f, 0.0f};
    const GLfloat lowerTip[] = {0.0f, -0.60f, 0.0f};
    const GLfloat ring[][3] = {
        {0.42f, 0.0f, 0.0f},
        {0.21f, 0.0f, 0.36f},
        {-0.21f, 0.0f, 0.36f},
        {-0.42f, 0.0f, 0.0f},
        {-0.21f, 0.0f, -0.36f},
        {0.21f, 0.0f, -0.36f}
    };
    const GLfloat faceColors[][3] = {
        {0.30f, 0.90f, 1.00f},
        {0.72f, 0.58f, 0.98f},
        {0.58f, 0.72f, 1.00f},
        {0.62f, 0.35f, 0.88f},
        {0.78f, 0.88f, 1.00f},
        {0.98f, 0.80f, 0.36f}
    };

    glBegin(GL_TRIANGLES);
    for (int side = 0; side < 6; ++side) {
        const int next = (side + 1) % 6;
        glColor3fv(faceColors[side]);
        glVertex3fv(upperTip);
        glVertex3fv(ring[side]);
        glVertex3fv(ring[next]);
    }
    for (int side = 0; side < 6; ++side) {
        const int next = (side + 1) % 6;
        glColor3fv(faceColors[(side + 2) % 6]);
        glVertex3fv(lowerTip);
        glVertex3fv(ring[next]);
        glVertex3fv(ring[side]);
    }
    glEnd();

    glPopMatrix();

    if (lightingWasEnabled) glEnable(GL_LIGHTING);
}

}
