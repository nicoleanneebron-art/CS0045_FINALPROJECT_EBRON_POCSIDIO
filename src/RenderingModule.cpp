#include <GL/glew.h>
#include <GL/glut.h>

#include "RenderingModule.h"

namespace rendering {

void setMaterial(const Color& diffuse, const Color& ambient, float shininess) {
    const GLfloat diffuseValue[] = {diffuse.r, diffuse.g, diffuse.b, 1.0f};
    const GLfloat ambientValue[] = {ambient.r, ambient.g, ambient.b, 1.0f};
    const GLfloat specularValue[] = {0.10f, 0.13f, 0.08f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseValue);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientValue);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularValue);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void initializeLighting() {
    const GLfloat ambient[] = {0.28f, 0.34f, 0.38f, 1.0f};
    const GLfloat diffuse[] = {1.0f, 0.93f, 0.72f, 1.0f};
    const GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.75f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.025f);
}

}
