#include "SceneState.h"

namespace scene_state {

int windowWidth = 1100;
int windowHeight = 700;
float elapsedSeconds = 0.0f;
float lastFrameSeconds = 0.0f;
bool keys[256] = {};
bool islandOrbiting = false;

float cameraX = 0.0f;
float cameraY = 3.0f;
float cameraZ = 8.8f;
float cameraYaw = -90.0f;
float cameraPitch = -12.0f;
float cameraFieldOfView = 58.0f;
int lastMouseX = 0;
int lastMouseY = 0;
bool mouseLookInitialized = false;
bool mouseInsideWindow = false;
bool groundLevelView = false;

GLuint gridVBO = 0;
GLsizei gridVertexCount = 0;
GLuint moundVBO = 0;
GLsizei moundVertexCount = 0;
GLuint islandVBO = 0;
GLsizei islandVertexCount = 0;
GLuint centralTreeVBO = 0;
GLsizei centralTreeVertexCount = 0;
GLuint landVBO = 0;
GLsizei landVertexCount = 0;
GLuint rockVBO = 0;
GLsizei rockVertexCount = 0;
GLuint propVBO = 0;
GLsizei propVertexCount = 0;
GLuint waterVBO = 0;
GLsizei waterVertexCount = 0;
GLuint waterTexture = 0;
GLuint grassVBO = 0;
GLsizei grassVertexCount = 0;
GLuint islandGrassVBO = 0;
GLsizei islandGrassVertexCount = 0;
std::vector<MeshVertex> islandGrassBaseVertices;
std::vector<MeshVertex> animatedIslandGrassVertices;
std::vector<WaterVertex> animatedWaterVertices;

}
