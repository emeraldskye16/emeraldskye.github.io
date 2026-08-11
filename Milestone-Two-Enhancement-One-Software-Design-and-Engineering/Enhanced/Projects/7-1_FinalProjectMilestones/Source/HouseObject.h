// HouseObject.h
#pragma once

#include "SceneObject.h"

class HouseObject : public SceneObject
{
public:
    void loadResources(ShapeMeshes* meshes) override;
    void update(float deltaTime) override;
    void render(ShaderManager* shaderManager) override;

private:
    ShapeMeshes* m_meshes = nullptr;
};
