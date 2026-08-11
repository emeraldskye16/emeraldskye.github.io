#include "LighthouseObject.h"
#include "ShaderManager.h"
#include "ShapeMeshes.h"

void LighthouseObject::loadResources(ShapeMeshes* meshes)
{
    m_meshes = meshes;
    m_basicMeshes = meshes;   // important: base pointer
}

void LighthouseObject::update(float) {}

void LighthouseObject::render(ShaderManager* shaderManager)
{
    if (!shaderManager || !m_meshes) return;

    // Lighthouse Base
    scaleXYZ = glm::vec3(5.0f, 15.0f, 3.0f);
    XrotationDegrees = 7.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(0.0f, -10.0f, -10.0f);

    SetTransformations(shaderManager,
        scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
    SetShaderTexture(shaderManager, "walls");
    SetShaderMaterial(shaderManager, "walls");
    m_basicMeshes->DrawTaperedCylinderMesh();

    // Lighthouse Light
    scaleXYZ = glm::vec3(1.5f, 3.0f, 1.0f);
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(0.0f, 5.0f, -10.0f);

    SetTransformations(shaderManager,
        scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
    SetShaderColor(shaderManager, 0.4f, 0.4f, 0.4f, 1.0f);
    m_basicMeshes->DrawTaperedCylinderMesh();

    // Lighthouse Light Glass
    scaleXYZ = glm::vec3(4.3f, 17.6f, 3.0f);
    XrotationDegrees = 7.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(0.0f, -10.0f, -10.0f);

    SetTransformations(shaderManager,
        scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
    SetShaderColor(shaderManager, 1.0f, 1.0f, 1.0f, 0.1f);
    m_basicMeshes->DrawTaperedCylinderMesh();

    // Lighthouse Top
    scaleXYZ = glm::vec3(2.4f, 2.8f, 1.8f);
    XrotationDegrees = 10.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(0.0f, 7.5f, -8.0f);

    SetTransformations(shaderManager,
        scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
    SetShaderTexture(shaderManager, "roof");
    SetShaderMaterial(shaderManager, "roof");
    m_basicMeshes->DrawConeMesh();
}
