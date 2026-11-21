#pragma once

#include <GL/glew.h>

#include <vector>

namespace scene_state {

struct MeshVertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

struct WaterVertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
    GLfloat s;
    GLfloat t;
};

extern int windowWidth;
extern int windowHeight;
extern float elapsedSeconds;
extern float lastFrameSeconds;
extern bool keys[256];

extern float cameraX;
extern float cameraY;
extern float cameraZ;
extern float cameraYaw;
extern float cameraPitch;
extern float cameraFieldOfView;
extern int lastMouseX;
extern int lastMouseY;
extern bool mouseLookInitialized;
extern bool mouseInsideWindow;
extern bool groundLevelView;

extern GLuint gridVBO;
extern GLsizei gridVertexCount;
extern GLuint moundVBO;
extern GLsizei moundVertexCount;
extern GLuint islandVBO;
extern GLsizei islandVertexCount;
extern GLuint landVBO;
extern GLsizei landVertexCount;
extern GLuint waterVBO;
extern GLsizei waterVertexCount;
extern GLuint waterTexture;
extern GLuint grassVBO;
extern GLsizei grassVertexCount;
extern GLuint islandGrassVBO;
extern GLsizei islandGrassVertexCount;
extern std::vector<MeshVertex> islandGrassBaseVertices;
extern std::vector<MeshVertex> animatedIslandGrassVertices;
extern std::vector<MeshVertex> grassBaseVertices;
extern std::vector<MeshVertex> animatedGrassVertices;
extern std::vector<WaterVertex> animatedWaterVertices;
extern unsigned int grassAnimationFrame;

}
