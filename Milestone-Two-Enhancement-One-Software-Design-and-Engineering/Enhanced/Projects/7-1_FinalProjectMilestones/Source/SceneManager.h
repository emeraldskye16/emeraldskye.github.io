///////////////////////////////////////////////////////////////////////////////
// shadermanager.h
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Updated by Emerald Tresch for CS-330 Final Project
///////////////////////////////////////////////////////////////////////////////
// SceneManager.h  (key changes only)
#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShaderManager.h"
#include "ShapeMeshes.h"
#include "TextureManager.h"
#include "SceneObject.h"
#include "LighthouseObject.h"
#include "HouseObject.h"
#include "MossRockObject.h"
#include "BoulderObject.h"

class SceneManager
{
public:
    // Simple POD for object material used by SceneObject and SceneManager
    struct OBJECT_MATERIAL
    {
        std::string tag;
        glm::vec3 ambientColor;
        float ambientStrength;
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        float shininess;
    };

    SceneManager(ShaderManager* pShaderManager);
    ~SceneManager();

    void PrepareScene();
    void RenderScene();

    // Texture helpers used inside SceneManager implementation
    bool CreateGLTexture(const char* filename, std::string tag);
    void BindGLTextures();
    void DestroyGLTextures();

    int  FindTextureID(std::string tag);
    int  FindTextureSlot(std::string tag);

    bool FindMaterial(std::string tag, OBJECT_MATERIAL& material);

    // Methods used by students / scene setup
    void LoadSceneTextures();
    void SetupSceneLights();
    void DefineObjectMaterials();

    // helpers used by objects / global accessors
    void SetTransformations(
        glm::vec3 scaleXYZ,
        float XrotationDegrees,
        float YrotationDegrees,
        float ZrotationDegrees,
        glm::vec3 positionXYZ);

    void SetShaderColor(
        float redColorValue,
        float greenColorValue,
        float blueColorValue,
        float alphaValue);

    void SetShaderTexture(std::string textureTag);
    void SetShaderMaterial(std::string materialTag);

    void SetTextureUVScale(float u, float v);

    // Added declarations to match implementations in SceneManager.cpp
    void SetShaderTexture2(std::string textureTag);
    void SetTextureBlend(float blendFactor);
    void BindAndSetBlendTextures(const std::string& texA, const std::string& texB, float blendFactor);
    void BindTextureToUnit(const std::string& textureTag, int unit);
    void SetShaderBlendTextures(const std::string& texTag1, const std::string& texTag2, float blend);

    // STATIC convenience wrappers so other classes (e.g. SceneObject)
    // can use SceneManager functionality without holding a pointer.
    // These call the singleton instance (s_instance). If no instance is present,
    // they return sensible defaults.
    static int  FindTextureIDStatic(const std::string& tag);
    static int  FindTextureSlotStatic(const std::string& tag);
    static bool FindMaterialStatic(const std::string& tag, OBJECT_MATERIAL& material);

    private:
    // singleton pointer set by constructor / cleared by destructor
    static SceneManager* s_instance;

    ShaderManager* m_pShaderManager{ nullptr };
    ShapeMeshes* m_basicMeshes{ nullptr };
    TextureManager m_textureManager;

    // Mirrors the design in the original SceneManager.cpp: local texture table
    TextureManager::TEXTURE_INFO m_textureIDs[16];
    int m_loadedTextures{ 0 };

    std::vector<OBJECT_MATERIAL> m_objectMaterials;

    // scene objects
    std::vector<SceneObject*> m_objects;

    // texture IDs kept around for convenience (original naming)
    GLuint m_LighthouseTex{ 0 };
    GLuint m_HouseWallTex{ 0 };
    GLuint m_HouseRoofTex{ 0 };
    GLuint m_RockTex{ 0 };
    GLuint m_TerrainTex{ 0 };

    LighthouseObject* m_Lighthouse{ nullptr };
    HouseObject* m_House{ nullptr };
    MossRockObject* m_RockLeft{ nullptr };
    MossRockObject* m_RockRight{ nullptr };
    BoulderObject* m_Terrain{ nullptr };

    // helper used internally by SceneManager.cpp
    GLuint LoadTexture(const std::string& path);
};
