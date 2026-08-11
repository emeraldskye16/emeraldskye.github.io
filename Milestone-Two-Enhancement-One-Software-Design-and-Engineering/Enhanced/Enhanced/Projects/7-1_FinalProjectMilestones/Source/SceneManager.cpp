///////////////////////////////////////////////////////////////////////////////
// SceneManager.cpp
// ============
// Manages the creation, updating, and rendering of 3D scene objects.
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//  Updated by Emerald Tresch for CS-330 Final Project
///////////////////////////////////////////////////////////////////////////////
#include "SceneManager.h"
#include "ShaderManager.h"
#include "ShapeMeshes.h"
#include "TextureManager.h"

#include "LighthouseObject.h"
#include "HouseObject.h"
#include "MossRockObject.h"
#include "BoulderObject.h"

#include <glm/gtx/transform.hpp>
#include <iostream>
#include <algorithm> // std::max

// define static singleton pointer
SceneManager* SceneManager::s_instance = nullptr;

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
    s_instance = this;

    m_pShaderManager = pShaderManager;
    m_basicMeshes = new ShapeMeshes();

    // m_textureManager and m_loadedTextures are default-initialized in the header
}

// ------------------------------------------------------------
// Destructor
// ------------------------------------------------------------
SceneManager::~SceneManager()
{
    // Destroy scene objects
    for (SceneObject* obj : m_objects)
        delete obj;

    m_objects.clear();

    delete m_basicMeshes;
    m_basicMeshes = nullptr;

    // Destroy any GL textures created via TextureManager
    DestroyGLTextures();

    // clear singleton pointer
    s_instance = nullptr;
}

// ------------------------------------------------------------
// Texture / texture-table helpers (implementations required by header)
// ------------------------------------------------------------
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
    bool ok = m_textureManager.CreateGLTexture(filename, tag);
    if (!ok) return false;

    int slot = m_textureManager.FindTextureSlot(tag);
    int id = m_textureManager.FindTextureID(tag);

    if (slot >= 0 && slot < static_cast<int>(std::size(m_textureIDs)))
    {
        m_textureIDs[slot].tag = tag;
        m_textureIDs[slot].ID = static_cast<uint32_t>(id);
        m_loadedTextures = std::max(m_loadedTextures, slot + 1);
    }
    return true;
}

void SceneManager::BindGLTextures()
{
    m_textureManager.BindGLTextures();
}

void SceneManager::DestroyGLTextures()
{
    m_textureManager.DestroyGLTextures();

    // clear local mirror table
    for (auto &ti : m_textureIDs)
    {
        ti.tag.clear();
        ti.ID = 0;
    }
    m_loadedTextures = 0;
}

int SceneManager::FindTextureID(std::string tag)
{
    return m_textureManager.FindTextureID(tag);
}

int SceneManager::FindTextureSlot(std::string tag)
{
    return m_textureManager.FindTextureSlot(tag);
}

bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
    for (const auto &m : m_objectMaterials)
    {
        if (m.tag == tag)
        {
            material = m;
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------
// STATIC wrappers
// ------------------------------------------------------------
int SceneManager::FindTextureIDStatic(const std::string& tag)
{
    if (!s_instance) return -1;
    return s_instance->FindTextureID(tag);
}

int SceneManager::FindTextureSlotStatic(const std::string& tag)
{
    if (!s_instance) return -1;
    return s_instance->FindTextureSlot(tag);
}

bool SceneManager::FindMaterialStatic(const std::string& tag, OBJECT_MATERIAL& material)
{
    if (!s_instance) return false;
    return s_instance->FindMaterial(tag, material);
}

// ------------------------------------------------------------
// BindTextureToUnit()
// (still needed for multi-texture blending)
// ------------------------------------------------------------
void SceneManager::BindTextureToUnit(const std::string& textureTag, int unit)
{
    int texID = FindTextureID(textureTag);
    if (texID < 0) return;

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texID);
}

// ------------------------------------------------------------
// SetShaderBlendTextures()
// (still needed if you use multi-texture blending)
// ------------------------------------------------------------
void SceneManager::SetShaderBlendTextures(
    const std::string& texTag1,
    const std::string& texTag2,
    float blend)
{
    if (!m_pShaderManager) return;

    blend = glm::clamp(blend, 0.0f, 1.0f);

    int texID1 = FindTextureID(texTag1);
    int texID2 = FindTextureID(texTag2);
    if (texID1 < 0 || texID2 < 0) return;

    m_pShaderManager->setBoolValue("bUseTexture", true);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID2);

    m_pShaderManager->setIntValue("objectTexture", 0);
    m_pShaderManager->setIntValue("objectTexture2", 1);
    m_pShaderManager->setFloatValue("textureBlend", blend);
}

// ------------------------------------------------------------
// LoadSceneTextures()
// (unchanged — still needed)
// ------------------------------------------------------------
void SceneManager::LoadSceneTextures()
{
    // Adjusted relative paths so they are correct when running from the Debug folder.
    CreateGLTexture("../../../Utilities/textures/dirt.jpg", "floor");
    CreateGLTexture("../../../Utilities/textures/dirtydrywall.jpg", "walls");
    CreateGLTexture("../../../Utilities/textures/red.jpg", "roof");
    CreateGLTexture("../../../Utilities/textures/roof2.jpg", "roof2");
    CreateGLTexture("../../../Utilities/textures/boulder.jpg", "boulder");
    CreateGLTexture("../../../Utilities/textures/mossyrock.jpg", "moss");

    BindGLTextures();
}

// ------------------------------------------------------------
// DefineObjectMaterials()
// (unchanged — still needed)
// ------------------------------------------------------------
void SceneManager::DefineObjectMaterials()
{
    m_objectMaterials.clear();

    OBJECT_MATERIAL floorMat;
    floorMat.ambientColor = glm::vec3(0.18f);
    floorMat.ambientStrength = 0.18f;
    floorMat.diffuseColor = glm::vec3(0.55f);
    floorMat.specularColor = glm::vec3(0.01f);
    floorMat.shininess = 0.15f;
    floorMat.tag = "floor";
    m_objectMaterials.push_back(floorMat);

    OBJECT_MATERIAL wallsMat;
    wallsMat.ambientColor = glm::vec3(0.08f);
    wallsMat.ambientStrength = 0.08f;
    wallsMat.diffuseColor = glm::vec3(0.55f);
    wallsMat.specularColor = glm::vec3(0.003f);
    wallsMat.shininess = 0.15f;
    wallsMat.tag = "walls";
    m_objectMaterials.push_back(wallsMat);

    OBJECT_MATERIAL roofMat;
    roofMat.ambientColor = glm::vec3(0.18f);
    roofMat.ambientStrength = 0.18f;
    roofMat.diffuseColor = glm::vec3(0.65f);
    roofMat.specularColor = glm::vec3(0.02f);
    roofMat.shininess = 0.50f;
    roofMat.tag = "roof";
    m_objectMaterials.push_back(roofMat);

    OBJECT_MATERIAL boulderMat;
    boulderMat.ambientColor = glm::vec3(0.18f);
    boulderMat.ambientStrength = 0.18f;
    boulderMat.diffuseColor = glm::vec3(0.55f);
    boulderMat.specularColor = glm::vec3(0.01f);
    boulderMat.shininess = 0.25f;
    boulderMat.tag = "boulder";
    m_objectMaterials.push_back(boulderMat);
}

// ------------------------------------------------------------
// SetupSceneLights()
// (unchanged — still needed)
// ------------------------------------------------------------
void SceneManager::SetupSceneLights()
{
    // use the same name the fragment shader expects
    m_pShaderManager->setBoolValue("bUseLighting", true);

    m_pShaderManager->setVec3Value("lightSources[0].position", 10.0f, 18.0f, -8.0f);
    m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.003f, 0.003f, 0.003f);
    m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 0.18f, 0.17f, 0.16f);
    m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.08f, 0.08f, 0.08f);
    m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 7.0f);
    m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.04f);

    m_pShaderManager->setVec3Value("lightSources[1].position", 12.0f, -6.0f, -10.0f);
    m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.002f, 0.002f, 0.002f);
    m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.10f, 0.10f, 0.12f);
    m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.01f, 0.01f, 0.01f);
    m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 3.5f);
    m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.01f);

    m_pShaderManager->setVec3Value("lightSources[2].position", 0.0f, 8.0f, 10.0f);
    m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.00f, 0.00f, 0.00f);
    m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.03f, 0.03f, 0.03f);
    m_pShaderManager->setVec3Value("lightSources[2].specularColor", 0.02f, 0.02f, 0.02f);
    m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 2.5f);
    m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.01f);

    m_pShaderManager->setVec3Value("lightSources[3].position", 0.0f, 0.0f, 0.0f);
    m_pShaderManager->setVec3Value("lightSources[3].ambientColor", 0.0f, 0.0f, 0.0f);
    m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", 0.0f, 0.0f, 0.0f);
    m_pShaderManager->setVec3Value("lightSources[3].specularColor", 0.0f, 0.0f, 0.0f);
    m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 1.0f);
    m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.0f);
}

// ------------------------------------------------------------
// PrepareScene()
// Loads textures, materials, lights, meshes, and objects
// ------------------------------------------------------------
void SceneManager::PrepareScene()
{
    // 1. Load textures
    m_textureManager.LoadSceneTextures();

    // 2. Define materials
    DefineObjectMaterials();

    // 3. Setup lights
    SetupSceneLights();

    // 4. Load all basic meshes once
    m_basicMeshes->LoadPlaneMesh();
    m_basicMeshes->LoadCylinderMesh();
    m_basicMeshes->LoadSphereMesh();
    m_basicMeshes->LoadBoxMesh();
    m_basicMeshes->LoadConeMesh();
    m_basicMeshes->LoadPrismMesh();
    m_basicMeshes->LoadTaperedCylinderMesh();
    m_basicMeshes->LoadTorusMesh();

    // 5. Create objects
    LighthouseObject* lighthouse = new LighthouseObject();
    lighthouse->loadResources(m_basicMeshes);
    m_objects.push_back(lighthouse);

    HouseObject* house = new HouseObject();
    house->loadResources(m_basicMeshes);
    m_objects.push_back(house);

    MossRockObject* mossLeft = new MossRockObject();
    mossLeft->loadResources(m_basicMeshes);
    m_objects.push_back(mossLeft);

    MossRockObject* mossRight = new MossRockObject();
    mossRight->loadResources(m_basicMeshes);
    m_objects.push_back(mossRight);

    BoulderObject* boulders = new BoulderObject();
    boulders->loadResources(m_basicMeshes);
    m_objects.push_back(boulders);
}

// ------------------------------------------------------------
// RenderScene()
// OOP version — calls each object’s render()
// ------------------------------------------------------------
void SceneManager::RenderScene()
{
    for (SceneObject* obj : m_objects)
    {
        obj->update(0.0f);
        obj->render(m_pShaderManager);
    }
}