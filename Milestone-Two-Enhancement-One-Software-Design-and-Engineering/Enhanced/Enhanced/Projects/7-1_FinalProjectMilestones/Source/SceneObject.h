// SceneObject.h
#pragma once

#include <glm/glm.hpp>
#include <string>

class ShaderManager;
class ShapeMeshes;

class SceneObject
{
public:
    virtual ~SceneObject() = default;

    virtual void loadResources(ShapeMeshes* meshes) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(ShaderManager* shaderManager) = 0;

protected:
    // shared transform state
    glm::vec3 scaleXYZ{ 1.0f, 1.0f, 1.0f };
    glm::vec3 positionXYZ{ 0.0f, 0.0f, 0.0f };
    float XrotationDegrees{ 0.0f };
    float YrotationDegrees{ 0.0f };
    float ZrotationDegrees{ 0.0f };

    ShapeMeshes* m_basicMeshes{ nullptr };

    // helpers that mirror the original SceneManager logic
    void SetTransformations(
        ShaderManager* shader,
        glm::vec3 scale,
        float xDeg,
        float yDeg,
        float zDeg,
        glm::vec3 pos);

    void SetShaderColor(
        ShaderManager* shader,
        float r, float g, float b, float a);

    void SetShaderTexture(
        ShaderManager* shader,
        const std::string& textureTag);

    void SetShaderMaterial(
        ShaderManager* shader,
        const std::string& materialTag);
};