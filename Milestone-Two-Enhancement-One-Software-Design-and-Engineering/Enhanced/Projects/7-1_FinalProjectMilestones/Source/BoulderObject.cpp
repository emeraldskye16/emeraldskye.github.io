// BoulderObject.cpp
#include "BoulderObject.h"
#include "ShaderManager.h"
#include "ShapeMeshes.h"
#include <glm/gtx/transform.hpp>

void BoulderObject::loadResources(ShapeMeshes* meshes)
{
    m_meshes = meshes;
    m_basicMeshes = meshes;   // IMPORTANT: base class pointer
}

void BoulderObject::update(float) {}

void BoulderObject::render(ShaderManager* shaderManager)
{
    if (!shaderManager || !m_meshes) return;

    // ---------------------------------------------------------
    // Boulder 1 Top
    // ---------------------------------------------------------
    scaleXYZ = glm::vec3(5.5f, 7.5f, 3.0f);
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(-12.5f, -20.0f, 1.5f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "boulder");
    SetShaderMaterial(shaderManager, "floor");

    m_basicMeshes->DrawSphereMesh();


    // ---------------------------------------------------------
    // Boulder 2 Base
    // ---------------------------------------------------------
    scaleXYZ = glm::vec3(12.0f, 17.0f, 3.0f);
    XrotationDegrees = 0.0f;
    YrotationDegrees = -7.5f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(-25.0f, -27.0f, -5.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "boulder");
    SetShaderMaterial(shaderManager, "floor");

    m_basicMeshes->DrawTaperedCylinderMesh();


    // ---------------------------------------------------------
    // Boulder 2 Top
    // ---------------------------------------------------------
    scaleXYZ = glm::vec3(5.5f, 7.5f, 3.0f);
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(-25.0f, -12.0f, -5.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "boulder");
    SetShaderMaterial(shaderManager, "floor");

    m_basicMeshes->DrawSphereMesh();
}