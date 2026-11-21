#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GL/glut.h>

#include "SceneConfig.h"
#include "SceneState.h"
#include "RenderingModule.h"
#include "GridModule.h"
#include "CameraModule.h"
#include "WaterModule.h"
#include "FloatingIslandsModule.h"
#include "TerrainModule.h"
#include "SkyModule.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {
using namespace scene_state;
constexpr float PI = scene::PI;
constexpr float DEG_TO_RAD = scene::DEG_TO_RAD;

MeshVertex makeHemisphereVertex(float elevation, float angle) {
    const float horizontalRadius = std::cos(elevation);
    return {
        horizontalRadius * std::cos(angle),
        std::sin(elevation),
        horizontalRadius * std::sin(angle),
        horizontalRadius * std::cos(angle),
        std::sin(elevation),
        horizontalRadius * std::sin(angle)
    };
}

MeshVertex makeColoredMoundVertex(float elevation, float angle) {
    MeshVertex vertex = makeHemisphereVertex(elevation, angle);
    const float patch = 0.5f + 0.5f * std::sin(angle * 3.0f + elevation * 5.0f);
    vertex.r = 0.16f + 0.13f * patch;
    vertex.g = 0.34f + 0.25f * patch;
    vertex.b = 0.065f + 0.075f * patch;
    return vertex;
}

MeshVertex makeSphereVertex(float latitude, float longitude) {
    const float horizontalRadius = std::cos(latitude);
    return {
        horizontalRadius * std::cos(longitude),
        std::sin(latitude),
        horizontalRadius * std::sin(longitude),
        horizontalRadius * std::cos(longitude),
        std::sin(latitude),
        horizontalRadius * std::sin(longitude)
    };
}

void buildGeometryVBOs() {
    std::vector<MeshVertex> gridVertices;
    constexpr int gridExtent = 24;
    constexpr float gridHeight = -2.75f;
    for (int index = -gridExtent; index <= gridExtent; ++index) {
        const float coordinate = static_cast<float>(index);
        gridVertices.push_back({coordinate, gridHeight, -gridExtent, 0, 1, 0});
        gridVertices.push_back({coordinate, gridHeight, gridExtent, 0, 1, 0});
        gridVertices.push_back({-gridExtent, gridHeight, coordinate, 0, 1, 0});
        gridVertices.push_back({gridExtent, gridHeight, coordinate, 0, 1, 0});
    }

    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(gridVertices.size() * sizeof(MeshVertex)),
                 gridVertices.data(), GL_STATIC_DRAW);
    gridVertexCount = static_cast<GLsizei>(gridVertices.size());

    std::vector<MeshVertex> moundVertices;
    constexpr int slices = 64;
    constexpr int rings = 24;
    for (int ring = 0; ring < rings; ++ring) {
        const float lowerElevation = (PI * 0.5f) * static_cast<float>(ring) / rings;
        const float upperElevation = (PI * 0.5f) * static_cast<float>(ring + 1) / rings;
        for (int slice = 0; slice < slices; ++slice) {
            const float lowerAngle = 2.0f * PI * static_cast<float>(slice) / slices;
            const float upperAngle = 2.0f * PI * static_cast<float>(slice + 1) / slices;
            const MeshVertex a = makeColoredMoundVertex(lowerElevation, lowerAngle);
            const MeshVertex b = makeColoredMoundVertex(upperElevation, lowerAngle);
            const MeshVertex c = makeColoredMoundVertex(upperElevation, upperAngle);
            const MeshVertex d = makeColoredMoundVertex(lowerElevation, upperAngle);
            moundVertices.insert(moundVertices.end(), {a, b, c, a, c, d});
        }
    }

    // Close the hemisphere with a short cylindrical skirt and bottom cap.
    constexpr float moundSkirtDepth = 0.75f;
    for (int slice = 0; slice < slices; ++slice) {
        const float lowerAngle = 2.0f * PI * static_cast<float>(slice) / slices;
        const float upperAngle = 2.0f * PI * static_cast<float>(slice + 1) / slices;
        const MeshVertex topA = makeColoredMoundVertex(0.0f, lowerAngle);
        const MeshVertex topB = makeColoredMoundVertex(0.0f, upperAngle);
        const MeshVertex bottomA = {
            std::cos(lowerAngle), -moundSkirtDepth, std::sin(lowerAngle),
            std::cos(lowerAngle), 0.0f, std::sin(lowerAngle), 0.08f, 0.18f, 0.035f
        };
        const MeshVertex bottomB = {
            std::cos(upperAngle), -moundSkirtDepth, std::sin(upperAngle),
            std::cos(upperAngle), 0.0f, std::sin(upperAngle), 0.08f, 0.18f, 0.035f
        };
        moundVertices.insert(moundVertices.end(),
                             {topA, bottomA, bottomB, topA, bottomB, topB});

        const MeshVertex centerBottom = {0.0f, -moundSkirtDepth, 0.0f,
                                         0.0f, -1.0f, 0.0f,
                                         0.08f, 0.18f, 0.035f};
        moundVertices.insert(moundVertices.end(),
                             {centerBottom, bottomB, bottomA});
    }

    glGenBuffers(1, &moundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, moundVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(moundVertices.size() * sizeof(MeshVertex)),
                 moundVertices.data(), GL_STATIC_DRAW);
    moundVertexCount = static_cast<GLsizei>(moundVertices.size());

    std::vector<MeshVertex> islandVertices;
    constexpr int islandSlices = 32;
    constexpr float islandApexY = -1.55f;
    const MeshVertex topCenter = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                  0.28f, 0.52f, 0.14f};
    const float layerY[] = {0.0f, -0.24f, -0.55f, -0.91f, -1.22f};
    const float layerRadius[] = {1.0f, 0.93f, 0.72f, 0.48f, 0.25f};
    const float layerRed[] = {0.19f, 0.27f, 0.24f, 0.19f, 0.13f};
    const float layerGreen[] = {0.38f, 0.30f, 0.25f, 0.20f, 0.15f};
    const float layerBlue[] = {0.11f, 0.17f, 0.17f, 0.15f, 0.13f};
    auto rockVertex = [&](int layer, int slice) {
        const float angle = 2.0f * PI * static_cast<float>(slice) / islandSlices;
        const float irregularity = 1.0f +
            0.13f * std::sin(angle * 3.0f + layer * 1.7f) +
            0.075f * std::cos(angle * 5.0f - layer * 0.9f) +
            0.035f * std::sin(angle * 8.0f + layer * 2.3f);
        const float radius = layerRadius[layer] * irregularity;
        const float x = std::cos(angle) * radius;
        const float z = std::sin(angle) * radius;
        const float nextRadius = layer == 4 ? 0.0f : layerRadius[layer + 1];
        const float slope = radius - nextRadius;
        return MeshVertex{x, layerY[layer], z,
                          std::cos(angle), slope * 0.8f, std::sin(angle),
                          layerRed[layer], layerGreen[layer], layerBlue[layer]};
    };

    // Circular grassy top disc with an organic edge.
    for (int slice = 0; slice < islandSlices; ++slice) {
        const MeshVertex topA = rockVertex(0, slice);
        const MeshVertex topB = rockVertex(0, (slice + 1) % islandSlices);
        islandVertices.insert(islandVertices.end(), {topCenter, topA, topB});
    }

    // Uneven strata descend toward a narrow, jagged underside.
    for (int layer = 0; layer < 4; ++layer) {
        for (int slice = 0; slice < islandSlices; ++slice) {
            const int next = (slice + 1) % islandSlices;
            const MeshVertex a = rockVertex(layer, slice);
            const MeshVertex b = rockVertex(layer + 1, slice);
            const MeshVertex c = rockVertex(layer + 1, next);
            const MeshVertex d = rockVertex(layer, next);
            islandVertices.insert(islandVertices.end(), {a, b, c, a, c, d});
        }
    }

    const MeshVertex apex = {0.08f, islandApexY, -0.05f, 0.0f, -1.0f, 0.0f,
                             0.09f, 0.11f, 0.10f};
    for (int slice = 0; slice < islandSlices; ++slice) {
        const MeshVertex a = rockVertex(4, slice);
        const MeshVertex b = rockVertex(4, (slice + 1) % islandSlices);
        islandVertices.insert(islandVertices.end(), {a, apex, b});
    }

    glGenBuffers(1, &islandVBO);
    glBindBuffer(GL_ARRAY_BUFFER, islandVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(islandVertices.size() * sizeof(MeshVertex)),
                 islandVertices.data(), GL_STATIC_DRAW);
    islandVertexCount = static_cast<GLsizei>(islandVertices.size());

    std::vector<MeshVertex> landVertices;
    std::vector<MeshVertex> grassVertices;
    constexpr float cellSize = 0.25f;
    constexpr int landCells = 240;
    constexpr float lakeRadius = 3.65f;
    constexpr float landBottomY = -5.50f;
    const auto terrainHeight = [](float x, float z) {
        const float distance = std::sqrt(x * x + z * z);
        const float patchNoise = std::sin(x * 0.62f) +
                                 std::cos(z * 0.48f) +
                                 0.55f * std::sin((x - z) * 0.35f);
        const float patchEdgeDip = std::fmax(0.0f,
            std::fmin(1.0f, (0.25f - patchNoise) / 0.60f));
        const float lakeEdgeDip = std::fmax(0.0f,
            std::fmin(1.0f, (4.85f - distance) / 1.20f));
        const float outerBasin = std::fmax(0.0f,
            std::fmin(1.0f, (distance - 11.0f) / 7.0f));
        return -2.67f + 0.16f * std::sin(x * 1.35f) +
               0.12f * std::cos(z * 1.10f) +
               0.07f * std::sin((x + z) * 2.20f) -
               0.18f * patchEdgeDip -
               0.24f * lakeEdgeDip -
               0.30f * outerBasin;
    };
    const auto addLandVertex = [&](float x, float y, float z,
                                   float nx, float ny, float nz) {
        const float distance = std::sqrt(x * x + z * z);
        const float outerBasin = std::fmax(0.0f,
            std::fmin(1.0f, (distance - 11.0f) / 7.0f));
        const float patchNoise = std::sin(x * 0.62f) +
                                 std::cos(z * 0.48f) +
                                 0.55f * std::sin((x - z) * 0.35f);
        const float lakeBlend = std::fmax(0.0f, std::fmin(1.0f,
            (distance - lakeRadius) / 0.85f));
        const float patchBlend = std::fmax(0.0f, std::fmin(1.0f,
            (patchNoise + 0.35f) / 0.60f));
        float waterBlend = std::fmin(lakeBlend, patchBlend);
        waterBlend = std::fmin(waterBlend, 1.0f - 0.75f * outerBasin);
        waterBlend = waterBlend * waterBlend * (3.0f - 2.0f * waterBlend);
        const float toneNoise = 0.5f + 0.5f *
            std::sin(x * 0.22f + 1.4f * std::cos(z * 0.17f));
        const float warmTone = 0.5f + 0.5f * std::sin(x * 0.13f - z * 0.19f);
        float red = 0.08f + 0.20f * toneNoise;
        float green = 0.20f + 0.24f * toneNoise;
        float blue = 0.045f + 0.055f * toneNoise;
        if (warmTone > 0.72f) {
            red += 0.07f;
            green -= 0.025f;
            blue -= 0.012f;
        }
        const float edgeShade = 0.62f + 0.38f * waterBlend;
        red *= edgeShade;
        green *= edgeShade;
        blue *= edgeShade;
        landVertices.push_back({x, y, z,
                                nx, ny, nz,
                                red, green, blue});
    };
    const auto addGrassVertex = [&](float x, float y, float z,
                                    float nx, float ny, float nz,
                                    float red, float green, float blue) {
        grassVertices.push_back({x, y, z, nx, ny, nz,
                                 red, green, blue});
    };

    // Irregular land patches leave a central lake and exposed grid channels.
    for (int row = -landCells / 2; row < landCells / 2; ++row) {
        for (int column = -landCells / 2; column < landCells / 2; ++column) {
            const float x = column * cellSize;
            const float z = row * cellSize;
            const float centerX = x + cellSize * 0.5f;
            const float centerZ = z + cellSize * 0.5f;
            const float distance = std::sqrt(centerX * centerX + centerZ * centerZ);
            const float edgeLakeNoise = std::sin(centerX * 0.35f) +
                                        std::cos(centerZ * 0.28f) +
                                        0.45f * std::sin((centerX + centerZ) * 0.22f);
            const float patchNoise = std::sin(centerX * 0.62f) +
                                     std::cos(centerZ * 0.48f) +
                                     0.55f * std::sin((centerX - centerZ) * 0.35f);
            const bool centralLake = distance < lakeRadius;
            const bool outerLake = distance > 14.0f && edgeLakeNoise > 0.70f;
            const bool patchLake = patchNoise <= -0.70f;
            if (centralLake || outerLake || patchLake) {
                continue;
            }
            const float x1 = x;
            const float x2 = x + cellSize;
            const float z1 = z;
            const float z2 = z + cellSize;
            const float y11 = terrainHeight(x1, z1);
            const float y12 = terrainHeight(x1, z2);
            const float y22 = terrainHeight(x2, z2);
            const float y21 = terrainHeight(x2, z1);

            const int cellHash = (row * 92837111) ^ (column * 689287499);
            const unsigned int randomCell = static_cast<unsigned int>(cellHash);
            const float jitterX = (static_cast<float>(randomCell % 100u) / 100.0f - 0.5f) * cellSize * 0.65f;
            const float jitterZ = (static_cast<float>((randomCell / 101u) % 100u) / 100.0f - 0.5f) * cellSize * 0.65f;
            const float grassX = centerX + jitterX;
            const float grassZ = centerZ + jitterZ;
            const float grassY = terrainHeight(grassX, grassZ);
            const float heightVariation = static_cast<float>(randomCell % 100u) / 100.0f;
            const float widthVariation = static_cast<float>((randomCell / 17u) % 100u) / 100.0f;
            const float bladeHeight = 0.16f + 0.16f * heightVariation;
            const float bladeWidth = 0.025f + 0.018f * widthVariation;
            const float leanX = (static_cast<float>((randomCell / 29u) % 100u) / 100.0f - 0.5f) * 0.14f;
            const float leanZ = (static_cast<float>((randomCell / 37u) % 100u) / 100.0f - 0.5f) * 0.14f;
            const unsigned int grassPalette = randomCell % 5u;
            const float grassRed[] = {0.12f, 0.20f, 0.30f, 0.24f, 0.15f};
            const float grassGreen[] = {0.34f, 0.46f, 0.52f, 0.40f, 0.29f};
            const float grassBlue[] = {0.055f, 0.08f, 0.10f, 0.045f, 0.035f};
            const float tintRed = grassRed[grassPalette];
            const float tintGreen = grassGreen[grassPalette];
            const float tintBlue = grassBlue[grassPalette];

            const auto addCurvedGrassBlade = [&](float angle, float height, float width,
                                                 float lean, float red, float green, float blue) {
                constexpr int segments = 3;
                for (int segment = 0; segment < segments; ++segment) {
                    const float t0 = static_cast<float>(segment) / segments;
                    const float t1 = static_cast<float>(segment + 1) / segments;
                    const float curve0 = std::sin(t0 * PI * 0.5f) * lean;
                    const float curve1 = std::sin(t1 * PI * 0.5f) * lean;
                    const float center0X = grassX + std::cos(angle) * curve0;
                    const float center0Z = grassZ + std::sin(angle) * curve0;
                    const float center1X = grassX + std::cos(angle) * curve1;
                    const float center1Z = grassZ + std::sin(angle) * curve1;
                    const float width0 = width * (1.0f - t0 * 0.90f);
                    const float width1 = width * (1.0f - t1 * 0.90f);
                    const float sideX = -std::sin(angle);
                    const float sideZ =  std::cos(angle);
                    const float y0 = grassY + height * t0;
                    const float y1 = grassY + height * t1;

                    addGrassVertex(center0X - sideX * width0, y0, center0Z - sideZ * width0,
                                   0, 1, 0, red, green, blue);
                    addGrassVertex(center1X - sideX * width1, y1, center1Z - sideZ * width1,
                                   0, 1, 0, red, green, blue);
                    addGrassVertex(center1X + sideX * width1, y1, center1Z + sideZ * width1,
                                   0, 1, 0, red, green, blue);
                    addGrassVertex(center0X - sideX * width0, y0, center0Z - sideZ * width0,
                                   0, 1, 0, red, green, blue);
                    addGrassVertex(center1X + sideX * width1, y1, center1Z + sideZ * width1,
                                   0, 1, 0, red, green, blue);
                    addGrassVertex(center0X + sideX * width0, y0, center0Z + sideZ * width0,
                                   0, 1, 0, red, green, blue);
                }
            };

            // Curved tapered blades form a compact natural tuft.
            addCurvedGrassBlade(0.10f, bladeHeight, bladeWidth, leanX,
                                tintRed, tintGreen, tintBlue);
            addCurvedGrassBlade(2.15f, bladeHeight * 0.84f, bladeWidth * 0.88f, leanZ,
                                tintRed + 0.035f, tintGreen + 0.045f, tintBlue + 0.012f);
            addCurvedGrassBlade(1.35f, bladeHeight * 0.92f, bladeWidth * 0.80f, -leanX,
                                tintRed + 0.020f, tintGreen + 0.030f, tintBlue);
            addCurvedGrassBlade(3.00f, bladeHeight * 0.78f, bladeWidth * 0.76f, -leanZ,
                                tintRed - 0.010f, tintGreen - 0.020f, tintBlue);
            addCurvedGrassBlade(4.50f, bladeHeight * 0.88f, bladeWidth * 0.82f, leanX,
                                tintRed + 0.015f, tintGreen + 0.020f, tintBlue + 0.008f);
            addCurvedGrassBlade(5.45f, bladeHeight * 0.70f, bladeWidth * 0.68f, leanZ,
                                tintRed - 0.020f, tintGreen - 0.030f, tintBlue);
            // Top surface.
            addLandVertex(x1, y11, z1, 0.0f, 1.0f, 0.0f);
            addLandVertex(x1, y12, z2, 0.0f, 1.0f, 0.0f);
            addLandVertex(x2, y22, z2, 0.0f, 1.0f, 0.0f);
            addLandVertex(x1, y11, z1, 0.0f, 1.0f, 0.0f);
            addLandVertex(x2, y22, z2, 0.0f, 1.0f, 0.0f);
            addLandVertex(x2, y21, z1, 0.0f, 1.0f, 0.0f);

            // Four closed side walls.
            addLandVertex(x1, y11, z1, -1.0f, 0.0f, 0.0f);
            addLandVertex(x1, landBottomY, z1, -1.0f, 0.0f, 0.0f);
            addLandVertex(x1, landBottomY, z2, -1.0f, 0.0f, 0.0f);
            addLandVertex(x1, y11, z1, -1.0f, 0.0f, 0.0f);
            addLandVertex(x1, landBottomY, z2, -1.0f, 0.0f, 0.0f);
            addLandVertex(x1, y12, z2, -1.0f, 0.0f, 0.0f);

            addLandVertex(x2, y21, z1, 1.0f, 0.0f, 0.0f);
            addLandVertex(x2, y22, z2, 1.0f, 0.0f, 0.0f);
            addLandVertex(x2, landBottomY, z2, 1.0f, 0.0f, 0.0f);
            addLandVertex(x2, y21, z1, 1.0f, 0.0f, 0.0f);
            addLandVertex(x2, landBottomY, z2, 1.0f, 0.0f, 0.0f);
            addLandVertex(x2, landBottomY, z1, 1.0f, 0.0f, 0.0f);

            addLandVertex(x1, y11, z1, 0.0f, 0.0f, -1.0f);
            addLandVertex(x2, y21, z1, 0.0f, 0.0f, -1.0f);
            addLandVertex(x2, landBottomY, z1, 0.0f, 0.0f, -1.0f);
            addLandVertex(x1, y11, z1, 0.0f, 0.0f, -1.0f);
            addLandVertex(x2, landBottomY, z1, 0.0f, 0.0f, -1.0f);
            addLandVertex(x1, landBottomY, z1, 0.0f, 0.0f, -1.0f);

            addLandVertex(x1, y12, z2, 0.0f, 0.0f, 1.0f);
            addLandVertex(x1, landBottomY, z2, 0.0f, 0.0f, 1.0f);
            addLandVertex(x2, landBottomY, z2, 0.0f, 0.0f, 1.0f);
            addLandVertex(x1, y12, z2, 0.0f, 0.0f, 1.0f);
            addLandVertex(x2, landBottomY, z2, 0.0f, 0.0f, 1.0f);
            addLandVertex(x2, y22, z2, 0.0f, 0.0f, 1.0f);

            // Bottom cap closes the volume when viewed from below.
            addLandVertex(x1, landBottomY, z1, 0.0f, -1.0f, 0.0f);
            addLandVertex(x2, landBottomY, z2, 0.0f, -1.0f, 0.0f);
            addLandVertex(x1, landBottomY, z2, 0.0f, -1.0f, 0.0f);
            addLandVertex(x1, landBottomY, z1, 0.0f, -1.0f, 0.0f);
            addLandVertex(x2, landBottomY, z1, 0.0f, -1.0f, 0.0f);
            addLandVertex(x2, landBottomY, z2, 0.0f, -1.0f, 0.0f);
        }
    }

    // Fluffy grass clusters follow the curved tops of the mound range.
    const auto addMoundGrass = [&](float centerX, float centerZ, float radius,
                                   unsigned int seed) {
        for (int ring = 0; ring < 10; ++ring) {
            const int tuftCount = ring == 0 ? 1 : ring * 14;
            const float distance = ring == 0 ? 0.0f : radius * (0.11f * static_cast<float>(ring));
            for (int tuft = 0; tuft < tuftCount; ++tuft) {
                const float angle = static_cast<float>(tuft) * (2.0f * PI / tuftCount) +
                                    seed * 0.013f + ring * 0.21f;
                const float grassX = centerX + std::cos(angle) * distance;
                const float grassZ = centerZ + std::sin(angle) * distance;
                const float grassY = -2.75f +
                    std::sqrt(std::fmax(0.0f, radius * radius - distance * distance)) + 0.035f;
                const float bladeHeight = radius * (0.11f + 0.045f * static_cast<float>((seed + tuft + ring) % 5u));
                const float bladeWidth = radius * 0.032f;
                const float lean = radius * 0.10f;
            const float red[] = {0.12f, 0.20f, 0.24f, 0.16f, 0.28f};
            const float green[] = {0.34f, 0.46f, 0.42f, 0.39f, 0.50f};
            const float blue[] = {0.055f, 0.08f, 0.06f, 0.045f, 0.075f};
            const unsigned int palette = (seed + tuft) % 5u;
            const float angles[] = {0.10f, 1.35f, 2.15f, 3.00f, 4.50f};
            const float heightScale[] = {1.00f, 0.92f, 0.84f, 0.78f, 0.88f};
            const float widthScale[] = {1.00f, 0.80f, 0.88f, 0.76f, 0.82f};
            for (int blade = 0; blade < 5; ++blade) {
                const float bladeAngle = angles[blade] + seed * 0.017f;
                for (int segment = 0; segment < 3; ++segment) {
                    const float t0 = static_cast<float>(segment) / 3.0f;
                    const float t1 = static_cast<float>(segment + 1) / 3.0f;
                    const float curve0 = std::sin(t0 * PI * 0.5f) * lean;
                    const float curve1 = std::sin(t1 * PI * 0.5f) * lean;
                    const float center0X = grassX + std::cos(bladeAngle) * curve0;
                    const float center0Z = grassZ + std::sin(bladeAngle) * curve0;
                    const float center1X = grassX + std::cos(bladeAngle) * curve1;
                    const float center1Z = grassZ + std::sin(bladeAngle) * curve1;
                    const float sideX = -std::sin(bladeAngle);
                    const float sideZ = std::cos(bladeAngle);
                    const float width0 = bladeWidth * widthScale[blade] * (1.0f - t0 * 0.90f);
                    const float width1 = bladeWidth * widthScale[blade] * (1.0f - t1 * 0.90f);
                    const float y0 = grassY + bladeHeight * heightScale[blade] * t0;
                    const float y1 = grassY + bladeHeight * heightScale[blade] * t1;
                    const float r = red[palette] + blade * 0.012f;
                    const float g = green[palette] + blade * 0.018f;
                    const float b = blue[palette];
                    addGrassVertex(center0X - sideX * width0, y0, center0Z - sideZ * width0, 0, 1, 0, r, g, b);
                    addGrassVertex(center1X - sideX * width1, y1, center1Z - sideZ * width1, 0, 1, 0, r, g, b);
                    addGrassVertex(center1X + sideX * width1, y1, center1Z + sideZ * width1, 0, 1, 0, r, g, b);
                    addGrassVertex(center0X - sideX * width0, y0, center0Z - sideZ * width0, 0, 1, 0, r, g, b);
                    addGrassVertex(center1X + sideX * width1, y1, center1Z + sideZ * width1, 0, 1, 0, r, g, b);
                    addGrassVertex(center0X + sideX * width0, y0, center0Z + sideZ * width0, 0, 1, 0, r, g, b);
                }
            }
            }
        }
    };

    // Reusable VBO-backed grass patch for every floating island top.
    std::vector<MeshVertex> islandGrassVertices;
    const auto addIslandGrassVertex = [&](float x, float y, float z,
                                           float red, float green, float blue) {
        islandGrassVertices.push_back({x, y, z, 0.0f, 1.0f, 0.0f,
                                       red, green, blue});
    };
    for (int tuft = 0; tuft < 112; ++tuft) {
        const float angle = tuft * 2.39996323f;
        const float distance = 0.045f + 0.94f * std::sqrt((tuft + 0.5f) / 112.0f);
        const float grassX = std::cos(angle) * distance;
        const float grassZ = std::sin(angle) * distance;
        const float height = 0.065f + 0.055f * static_cast<float>(tuft % 4) / 3.0f;
        const float width = 0.008f + 0.005f * static_cast<float>((tuft * 7) % 4) / 3.0f;
        const float lean = 0.045f + 0.028f * static_cast<float>((tuft * 3) % 3) / 2.0f;
        const float red[] = {0.12f, 0.20f, 0.30f, 0.24f, 0.15f};
        const float green[] = {0.34f, 0.46f, 0.52f, 0.40f, 0.29f};
        const float blue[] = {0.055f, 0.08f, 0.10f, 0.045f, 0.035f};
        const int palette = tuft % 5;
        for (int blade = 0; blade < 7; ++blade) {
            const float bladeAngle = blade * 0.90f + angle * 0.17f;
            for (int segment = 0; segment < 3; ++segment) {
                const float t0 = static_cast<float>(segment) / 3.0f;
                const float t1 = static_cast<float>(segment + 1) / 3.0f;
                const float curve0 = std::sin(t0 * PI * 0.5f) * lean;
                const float curve1 = std::sin(t1 * PI * 0.5f) * lean;
                const float center0X = grassX + std::cos(bladeAngle) * curve0;
                const float center0Z = grassZ + std::sin(bladeAngle) * curve0;
                const float center1X = grassX + std::cos(bladeAngle) * curve1;
                const float center1Z = grassZ + std::sin(bladeAngle) * curve1;
                const float sideX = -std::sin(bladeAngle);
                const float sideZ = std::cos(bladeAngle);
                const float width0 = width * (1.0f - t0 * 0.90f);
                const float width1 = width * (1.0f - t1 * 0.90f);
                const float y0 = 0.025f + height * t0;
                const float y1 = 0.025f + height * t1;
                const float r = red[palette] + blade * 0.012f;
                const float g = green[palette] + blade * 0.018f;
                const float b = blue[palette];
                addIslandGrassVertex(center0X - sideX * width0, y0, center0Z - sideZ * width0, r, g, b);
                addIslandGrassVertex(center1X - sideX * width1, y1, center1Z - sideZ * width1, r, g, b);
                addIslandGrassVertex(center1X + sideX * width1, y1, center1Z + sideZ * width1, r, g, b);
                addIslandGrassVertex(center0X - sideX * width0, y0, center0Z - sideZ * width0, r, g, b);
                addIslandGrassVertex(center1X + sideX * width1, y1, center1Z + sideZ * width1, r, g, b);
                addIslandGrassVertex(center0X + sideX * width0, y0, center0Z + sideZ * width0, r, g, b);
            }
        }
    }

    std::vector<WaterVertex> waterVertices;
    constexpr int waterResolution = 48;
    constexpr float waterExtent = 30.0f;
    const auto addWaterVertex = [&](float x, float z) {
        waterVertices.push_back({x, water::waterSurfaceHeight(x, z), z,
                                 0.0f, 1.0f, 0.0f,
                                 x * 0.10f, z * 0.10f});
    };

    // One subdivided continuous water plane sits below the terrain and is
    // revealed naturally through the lake openings.
    const float waterCell = (waterExtent * 2.0f) / waterResolution;
    for (int row = 0; row < waterResolution; ++row) {
        for (int column = 0; column < waterResolution; ++column) {
            const float x1 = -waterExtent + column * waterCell;
            const float x2 = x1 + waterCell;
            const float z1 = -waterExtent + row * waterCell;
            const float z2 = z1 + waterCell;
            addWaterVertex(x1, z1);
            addWaterVertex(x1, z2);
            addWaterVertex(x2, z2);
            addWaterVertex(x1, z1);
            addWaterVertex(x2, z2);
            addWaterVertex(x2, z1);
        }
    }

    glGenBuffers(1, &landVBO);
    glBindBuffer(GL_ARRAY_BUFFER, landVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(landVertices.size() * sizeof(MeshVertex)),
                 landVertices.data(), GL_STATIC_DRAW);
    landVertexCount = static_cast<GLsizei>(landVertices.size());

    glGenBuffers(1, &grassVBO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    grassBaseVertices = grassVertices;
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(grassVertices.size() * sizeof(MeshVertex)),
                 grassVertices.data(), GL_DYNAMIC_DRAW);
    grassVertexCount = static_cast<GLsizei>(grassVertices.size());

    glGenBuffers(1, &islandGrassVBO);
    glBindBuffer(GL_ARRAY_BUFFER, islandGrassVBO);
    islandGrassBaseVertices = islandGrassVertices;
    animatedIslandGrassVertices = islandGrassVertices;
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(islandGrassVertices.size() * sizeof(MeshVertex)),
                 islandGrassVertices.data(), GL_DYNAMIC_DRAW);
    islandGrassVertexCount = static_cast<GLsizei>(islandGrassVertices.size());

    glGenBuffers(1, &waterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(waterVertices.size() * sizeof(WaterVertex)),
                 waterVertices.data(), GL_DYNAMIC_DRAW);
    waterVertexCount = static_cast<GLsizei>(waterVertices.size());

    std::vector<unsigned char> waterPixels(32 * 32 * 4);
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 32; ++x) {
            const float wave = 0.5f + 0.5f * std::sin(x * 0.55f + y * 0.35f);
            const int offset = (y * 32 + x) * 4;
            waterPixels[offset + 0] = static_cast<unsigned char>(72.0f + 35.0f * wave);
            waterPixels[offset + 1] = static_cast<unsigned char>(165.0f + 45.0f * wave);
            waterPixels[offset + 2] = static_cast<unsigned char>(215.0f + 35.0f * wave);
            waterPixels[offset + 3] = 205;
        }
    }
    glGenTextures(1, &waterTexture);
    glBindTexture(GL_TEXTURE_2D, waterTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, waterPixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sky::drawSky();
    glMatrixMode(GL_MODELVIEW);
    // Keep animation time independent from camera movement and input updates.
    scene_state::elapsedSeconds =
        static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    camera::applyExplorationCamera();

    GLfloat lightPosition[] = {-4.0f, 7.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPushMatrix();
    glScalef(scene::mapScale, scene::mapScale, scene::mapScale);
    water::drawWater();
    terrain::drawLandmass();
    terrain::drawGrass();
    floating_islands::drawFloatingIslandRings();
    glPopMatrix();

    glutSwapBuffers();
}

void initialize() {
    glClearColor(0.25f, 0.58f, 0.82f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    const GLfloat fogColor[] = {0.35f, 0.58f, 0.82f, 1.0f};
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, 20.0f);
    glFogf(GL_FOG_END, 72.0f);
    glHint(GL_FOG_HINT, GL_NICEST);

    rendering::initializeLighting();
    sky::initializeSky();
}
}

namespace application {

int run(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Dadah Range Prototype - Butterfly Exploration");

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed.\n";
        return 1;
    }

    initialize();
    buildGeometryVBOs();
    glutDisplayFunc(display);
    glutReshapeFunc(camera::reshape);
    glutKeyboardFunc(camera::keyboardDown);
    glutKeyboardUpFunc(camera::keyboardUp);
    glutSpecialFunc(camera::specialDown);
    glutMouseFunc(camera::mouseButton);
    glutPassiveMotionFunc(camera::mouseLook);
    glutEntryFunc(camera::mouseEntry);
    glutTimerFunc(16, camera::timer, 0);
    glutMainLoop();
    return 0;
}

}
