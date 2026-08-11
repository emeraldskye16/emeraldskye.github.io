// SceneObject.cpp
#include "SceneObject.h"
#include "ShaderManager.h"
#include "SceneManager.h"   

#include <glm/gtx/transform.hpp>

void SceneObject::SetTransformations(
    ShaderManager* shader,
    glm::vec3 scale,
    float xDeg,
    float yDeg,
    float zDeg,
    glm::vec3 pos)
{
    glm::mat4 model =
        glm::translate(pos) *
        glm::rotate(glm::radians(xDeg), glm::vec3(1, 0, 0)) *
        glm::rotate(glm::radians(yDeg), glm::vec3(0, 1, 0)) *
        glm::rotate(glm::radians(zDeg), glm::vec3(0, 0, 1)) *
        glm::scale(scale);

    shader->setMat4Value("model", model);
}

void SceneObject::SetShaderColor(
    ShaderManager* shader,
    float r, float g, float b, float a)
{
    // use the boolean API so the intent matches the shader type
    shader->setBoolValue("bUseTexture", false);
    shader->setVec4Value("objectColor", glm::vec4(r, g, b, a));
}

void SceneObject::SetShaderTexture(
    ShaderManager* shader,
    const std::string& textureTag)
{
    // set the intent using boolean uniform
    shader->setBoolValue("bUseTexture", true);

    // Ask SceneManager for the texture slot
    int slot = SceneManager::FindTextureSlotStatic(textureTag);
    int texID = SceneManager::FindTextureIDStatic(textureTag);

    if (slot < 0 || texID < 0)
    {
        // fallback: no texture found
        shader->setBoolValue("bUseTexture", false);
        return;
    }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texID);

    // tell the shader which texture unit the sampler should sample from
    shader->setIntValue("objectTexture", slot);
}

void SceneObject::SetShaderMaterial(
    ShaderManager* shader,
    const std::string& materialTag)
{
    SceneManager::OBJECT_MATERIAL mat;

    bool found = SceneManager::FindMaterialStatic(materialTag, mat);
    if (!found)
    {
        // fallback: simple white material
        shader->setVec3Value("material.ambientColor", glm::vec3(1.0f));
        shader->setFloatValue("material.ambientStrength", 0.2f);
        shader->setVec3Value("material.diffuseColor", glm::vec3(1.0f));
        shader->setVec3Value("material.specularColor", glm::vec3(0.2f));
        shader->setFloatValue("material.shininess", 32.0f);
        return;
    }

    shader->setVec3Value("material.ambientColor", mat.ambientColor);
    shader->setFloatValue("material.ambientStrength", mat.ambientStrength);
    shader->setVec3Value("material.diffuseColor", mat.diffuseColor);
    shader->setVec3Value("material.specularColor", mat.specularColor);
    shader->setFloatValue("material.shininess", mat.shininess);
}