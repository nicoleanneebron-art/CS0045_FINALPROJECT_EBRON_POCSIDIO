#pragma once

#include <GL/gl.h>

struct Color {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

namespace rendering {

void setMaterial(const Color& diffuse, const Color& ambient, float shininess = 8.0f);
void initializeLighting();

}
