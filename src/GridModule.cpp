#include <GL/glew.h>
#include <GL/glut.h>

#include "GridModule.h"
#include "SceneState.h"

namespace grid {

    void drawFloorGrid() {
        glDisable(GL_LIGHTING);
        glColor3f(0.34f, 0.47f, 0.60f);
        glLineWidth(1.0f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, scene_state::gridVBO);
        glVertexPointer(3, GL_FLOAT, sizeof(scene_state::MeshVertex), reinterpret_cast<void*>(0));
        glDrawArrays(GL_LINES, 0, scene_state::gridVertexCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glEnable(GL_LIGHTING);
    }

}
