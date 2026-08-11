// HouseObject.cpp
#include "HouseObject.h"
#include "ShaderManager.h"
#include "ShapeMeshes.h"
#include <glm/gtx/transform.hpp>

void HouseObject::loadResources(ShapeMeshes* meshes)
{
    m_basicMeshes = meshes;
}

void HouseObject::update(float) {}

void HouseObject::render(ShaderManager* shaderManager)
{
    if (!shaderManager || !m_basicMeshes) return;

    // Lighthouse house
    scaleXYZ = glm::vec3(10.0f, 5.0f, 3.0f);
    XrotationDegrees = -7.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(5.3f, -7.5f, -10.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "walls");
    SetShaderMaterial(shaderManager, "walls");

    m_basicMeshes->DrawBoxMesh();

    /****************************************************************/
    // Lighthouse House Roof
    scaleXYZ = glm::vec3(2.0f, 7.0f, 2.0f);
    XrotationDegrees = -22.0f;
    YrotationDegrees = 180.0f;
    ZrotationDegrees = 90.0f;
    positionXYZ = glm::vec3(6.0f, -4.5f, -10.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "roof");
    SetShaderMaterial(shaderManager, "roof");

    m_basicMeshes->DrawPrismMesh();

    /****************************************************************/
    // Lighthouse house chimney
    scaleXYZ = glm::vec3(1.0f, 9.0f, 1.0f);
    XrotationDegrees = -7.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(10.0f, -6.0f, -9.2f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "walls");
    SetShaderMaterial(shaderManager, "walls");

    m_basicMeshes->DrawBoxMesh();

    /****************************************************************/
    // Lighthouse house 2
    scaleXYZ = glm::vec3(6.5f, 5.0f, 3.0f);
    XrotationDegrees = -7.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(16.0f, -9.5f, -8.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "walls");
    SetShaderMaterial(shaderManager, "walls");

    m_basicMeshes->DrawBoxMesh();

    /****************************************************************/
    // Lighthouse House Roof 2
    scaleXYZ = glm::vec3(2.0f, 7.0f, 2.0f);
    XrotationDegrees = -22.0f;
    YrotationDegrees = 180.0f;
    ZrotationDegrees = 90.0f;
    positionXYZ = glm::vec3(15.5f, -6.2f, -7.8f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "roof2");
    SetShaderMaterial(shaderManager, "roof");

    m_basicMeshes->DrawPrismMesh();

    /****************************************************************/
    // Lighthouse Utility Box
    scaleXYZ = glm::vec3(2.0f, 2.0f, 2.0f);
    XrotationDegrees = -7.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(22.0f, -9.5f, -8.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "walls");
    SetShaderMaterial(shaderManager, "walls");

    m_basicMeshes->DrawBoxMesh();

    /****************************************************************/
    // Lighthouse house 3
    scaleXYZ = glm::vec3(6.5f, 5.5f, 2.0f);
    XrotationDegrees = -7.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;
    positionXYZ = glm::vec3(30.2f, -9.5f, -8.0f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "walls");
    SetShaderMaterial(shaderManager, "walls");

    m_basicMeshes->DrawBoxMesh();

    /****************************************************************/
    // Lighthouse House Roof 3
    scaleXYZ = glm::vec3(2.0f, 7.0f, 2.0f);
    XrotationDegrees = -22.0f;
    YrotationDegrees = 180.0f;
    ZrotationDegrees = 90.0f;
    positionXYZ = glm::vec3(29.5f, -6.2f, -7.8f);

    SetTransformations(
        shaderManager,
        scaleXYZ,
        XrotationDegrees,
        YrotationDegrees,
        ZrotationDegrees,
        positionXYZ);

    SetShaderTexture(shaderManager, "roof2");
    SetShaderMaterial(shaderManager, "roof");

    m_basicMeshes->DrawPrismMesh();
}