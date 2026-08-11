// MossRockObject.cpp
#include "MossRockObject.h"
#include "ShaderManager.h"
#include "ShapeMeshes.h"
#include <glm/gtx/transform.hpp>

void MossRockObject::loadResources(ShapeMeshes* meshes)
{
    m_meshes = meshes;
    m_basicMeshes = meshes;   // IMPORTANT: base class pointer
}

void MossRockObject::update(float) {}

void MossRockObject::render(ShaderManager* shaderManager)
{
    if (!shaderManager || !m_meshes) return;

    // -------------------------
    // Moss Rock 1
    // -------------------------
    scaleXYZ = glm::vec3(35.0f, 10.0f, 20.0f);
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(5.0f, -20.0f, -15.5f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "moss");
    SetShaderMaterial(shaderManager, "floor");

    m_basicMeshes->DrawTaperedCylinderMesh();


    // -------------------------
    // Moss Rock 2
    // -------------------------
    scaleXYZ = glm::vec3(35.0f, 10.0f, 20.0f);
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(20.0f, -20.0f, -13.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "moss");
    SetShaderMaterial(shaderManager, "floor");

    m_basicMeshes->DrawTaperedCylinderMesh();
}
