///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>
#include "ViewManager.h"

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();

	// initialize the texture collection
	for (int i = 0; i < 16; i++)
	{
		m_textureIDs[i].tag = "/0";
		m_textureIDs[i].ID = -1;
	}
	m_loadedTextures = 0;
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	// clear the allocated memory
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
	// destroy the created OpenGL textures
	DestroyGLTextures();
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;
	
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with "
				<< colorChannels << " channels" << std::endl;
			stbi_image_free(image);
			glBindTexture(GL_TEXTURE_2D, 0);
			
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		if (m_textureIDs[i].ID != 0)
		{
			glDeleteTextures(1, &m_textureIDs[i].ID);
			m_textureIDs[i].ID = 0;
		}
	}
	m_loadedTextures = 0;
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{

	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(std::string textureTag)
{

	if (NULL == m_pShaderManager) return;

	m_pShaderManager->setIntValue(g_UseTextureName, true);

	// Slot = texture unit index we’ll use (0..15)
	int slot = FindTextureSlot(textureTag);
	if (slot < 0) return;

	// texID = actual OpenGL texture object handle
	int texID = FindTextureID(textureTag);
	if (texID < 0) return;

	// Bind the texture to that unit right now (robust)
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, (GLuint)texID);

	m_pShaderManager->setIntValue(g_TextureValueName, slot);

}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{

	if (NULL == m_pShaderManager) return;

	OBJECT_MATERIAL material;
	bool bFound = FindMaterial(materialTag, material);
	if (!bFound) return;

	// These uniform names must match your fragment shader's material struct
	// Common CS330 naming pattern:
	m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor.x, material.ambientColor.y, material.ambientColor.z);
	m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
	m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor.x, material.diffuseColor.y, material.diffuseColor.z);
	m_pShaderManager->setVec3Value("material.specularColor", material.specularColor.x, material.specularColor.y, material.specularColor.z);
	m_pShaderManager->setFloatValue("material.shininess", material.shininess);

}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/*********************************************************
*BindAndSetBlendTextures()
*
* This is the main fix :
*-bind two textures to known units(0 and 1)
* -set sampler uniforms to unit indices(0 and 1)
* -set blend factor

***********************************************************/
void SceneManager::BindAndSetBlendTextures(
	const std::string& texA,
	const std::string& texB,
	float blendFactor)
{
	if (NULL == m_pShaderManager) return;

	// clamp
	if (blendFactor < 0.0f) blendFactor = 0.0f;
	if (blendFactor > 1.0f) blendFactor = 1.0f;

	m_pShaderManager->setIntValue(g_UseTextureName, true);

	// Bind both textures for THIS draw call (robust)
	BindTextureToUnit(texA, 0);
	BindTextureToUnit(texB, 1);

	// Tell the shader which units to sample from:
	// objectTexture  -> unit 0
	// objectTexture2 -> unit 1
	m_pShaderManager->setIntValue("objectTexture", 0);
	m_pShaderManager->setIntValue("objectTexture2", 1);

	// Blend factor uniform used by mix()
	m_pShaderManager->setFloatValue("textureBlend", blendFactor);
}

/***********************************************************
 *  NEW: BindTextureToUnit()
 *
 *  Robust helper for binding a named texture to a specific unit.
 ***********************************************************/
void SceneManager::BindTextureToUnit(const std::string& textureTag, int unit)
{
	int texID = FindTextureID(textureTag);
	if (texID < 0) return;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, (GLuint)texID);
}
void SceneManager::SetShaderBlendTextures(const std::string& texTag1,
	const std::string& texTag2,
	float blend)
{
	if (!m_pShaderManager) return;

	// Clamp blend to [0, 1]
	if (blend < 0.0f) blend = 0.0f;
	if (blend > 1.0f) blend = 1.0f;

	int texID1 = FindTextureID(texTag1);
	int texID2 = FindTextureID(texTag2);
	if (texID1 < 0 || texID2 < 0) return;

	m_pShaderManager->setBoolValue("bUseTexture", true);

	// texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID1);

	// texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texID2);

	// tell shader which units to use
	m_pShaderManager->setIntValue("objectTexture", 0);
	m_pShaderManager->setIntValue("objectTexture2", 1);
	
	// Set blend factor
	m_pShaderManager->setFloatValue("textureBlend", blend);

}

/***********************************************************
 *  LoadSceneTextures()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	/*** STUDENTS - add the code BELOW for loading the textures that ***/
	/*** will be used for mapping to objects in the 3D scene. Up to  ***/
	/*** 16 textures can be loaded per scene. Refer to the code in   ***/
	/*** the OpenGL Sample for help.                                 ***/
	bool bReturn = false;

	bReturn = CreateGLTexture(
		"../../Utilities/textures/dirt.jpg",
		"floor");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/dirtydrywall.jpg",
		"walls");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/red.jpg",
		"roof");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/roof2.jpg",
		"roof2");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/boulder.jpg",
		"boulder");
	
	bReturn = CreateGLTexture(
		"../../Utilities/textures/mossyrock.jpg",
		"moss");

	// after the texture image data is loaded into memory, the
	// loaded textures need to be bound to texture slots - there
	// are a total of 16 available slots for scene textures
	BindGLTextures();
}

/***********************************************************
* DefineObjectMaterials()
 *
* This method is used for configuring the various material
* se�ngs for all of the objects in the 3D scene.
***********************************************************/
void SceneManager::DefineObjectMaterials()
{

	m_objectMaterials.clear();

	// PLANE (ground) — reflective
	OBJECT_MATERIAL floorMaterials;
	floorMaterials.ambientColor = glm::vec3(0.18f, 0.18f, 0.18f);
	floorMaterials.ambientStrength = 0.18f;                 // small lift so shadows aren’t pitch black
	floorMaterials.diffuseColor = glm::vec3(.55f, 0.55f, 0.55f);
	floorMaterials.specularColor = glm::vec3(.01f, 0.01f, 0.01f); // MUCH less shine
	floorMaterials.shininess = 0.15f;                  // very low
	floorMaterials.tag = "floor";

	m_objectMaterials.push_back(floorMaterials);

	// WALLS (masonry) — mostly diffuse

	OBJECT_MATERIAL wallsMaterials;
	wallsMaterials.ambientColor = glm::vec3(0.08f, 0.08f, 0.08f); // tiny cool bias helps neutral lighting
	wallsMaterials.ambientStrength = 0.08f;                 // boosts wall visibility
	wallsMaterials.diffuseColor = glm::vec3(0.55f, 0.55f, 0.55f); // brighter diffuse = catches light better
	wallsMaterials.specularColor = glm::vec3(0.003f, 0.003f, 0.003f); // nearly matte
	wallsMaterials.shininess = 0.15f;                  // extremely low
	wallsMaterials.tag = "walls";

	m_objectMaterials.push_back(wallsMaterials);


	// ROOF (paint/tile) — mild specular

	OBJECT_MATERIAL roofMaterials;
	roofMaterials.ambientColor = glm::vec3(0.18f, 0.18f, 0.18f);
	roofMaterials.ambientStrength = 0.18f;
	roofMaterials.diffuseColor = glm::vec3(0.65f, 0.65f, 0.65f);
	roofMaterials.specularColor = glm::vec3(0.02f, 0.02f, 0.02f); // subtle highlight only
	roofMaterials.shininess = 0.50f;                 
	roofMaterials.tag = "roof";

	m_objectMaterials.push_back(roofMaterials);

	// rocks (ground) — reflective
	OBJECT_MATERIAL boulderMaterials;
	boulderMaterials.ambientColor = glm::vec3(0.18f, 0.18f, 0.18f);
	boulderMaterials.ambientStrength = 0.18f;                 // small lift so shadows aren’t pitch black
	boulderMaterials.diffuseColor = glm::vec3(.55f, 0.55f, 0.55f);
	boulderMaterials.specularColor = glm::vec3(.01f, 0.01f, 0.01f); // MUCH less shine
	boulderMaterials.shininess = 0.25f;                  // very low
	boulderMaterials.tag = "boulder";

	m_objectMaterials.push_back(boulderMaterials);

}

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// Enable lighting in shader
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	// Light 0 = "SUN" (high, warm, broad)
	// Position high above and offset so it casts across the lighthouse
	m_pShaderManager->setVec3Value("lightSources[0].position", 10.0f, 18.0f, -8.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.003f, 0.003f, 0.003f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 0.18f, 0.17f, 0.16f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.08f, 0.08f, 0.08f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 7.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.04f);


	// Light 1 = "WALL WASH" (near walls, neutral/cool, very soft)
	// Put it off to the side of the lighthouse/house to brighten vertical faces
	m_pShaderManager->setVec3Value("lightSources[1].position", 12.0f, -6.0f, -10.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.002f, 0.002f, 0.002f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.10f, 0.10f, 0.12f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.01f, 0.01f, 0.01f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 3.5f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.01f);


	// Light 2 = very subtle back fill / rim to separate silhouette
	m_pShaderManager->setVec3Value("lightSources[2].position", 0.0f, 8.0f, 10.0f);
	m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.00f, 0.00f, 0.00f);
	m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.03f, 0.03f, 0.03f);
	m_pShaderManager->setVec3Value("lightSources[2].specularColor", 0.02f, 0.02f, 0.02f);
	m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 2.5f);
	m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.01f);
	
	// Light Source [3] — Explicitly DISABLED

	// The fragment shader loops over TOTAL_LIGHTS = 4 unconditionally.
	// If lightSources[3] is not initialized, the shader will read undefined data,
	// which can result in unpredictable lighting (over‑bright a
	// reas or black meshes).
	//
	// To prevent this, we explicitly set ALL properties of lightSources[3] to zero
	// so that it contributes no ambient, diffuse, or specular lighting to the scene.
	// This effectively disables the light while keeping the shader logic intact.
	
	m_pShaderManager->setVec3Value("lightSources[3].position", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[3].ambientColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[3].specularColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 1.0f);
	m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.0f);

	m_pShaderManager->setBoolValue("bUseLighting", true);

}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{

	// Textures must be loaded BEFORE lighting is applied
	LoadSceneTextures();

	// Define Phong materials for scene objects
	DefineObjectMaterials();

	// Configure all light sources (called ONCE)
	SetupSceneLights();

	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadPrismMesh();
	m_basicMeshes->LoadTaperedCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
}

/***********************************************************
 *  SetShaderTexture2()
 *
 *  Sets a second texture (objectTexture2) into the shader
 *  for multi-texturing / blending.
 ***********************************************************/
void SceneManager::SetShaderTexture2(std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		int textureSlot = FindTextureSlot(textureTag);
		if (textureSlot >= 0)
		{
			// objectTexture2 must match your fragment shader uniform name
			m_pShaderManager->setSampler2DValue("objectTexture2", textureSlot);
		}
	}
}

/***********************************************************
 *  SetTextureBlend()
 *
 *  Sets the blend factor between objectTexture and
 *  objectTexture2 in the shader.
 ***********************************************************/
void SceneManager::SetTextureBlend(float blendFactor)
{
	if (NULL != m_pShaderManager)
	{
		// clamp to [0,1] just to be safe
		if (blendFactor < 0.0f) blendFactor = 0.0f;
		if (blendFactor > 1.0f) blendFactor = 1.0f;

		m_pShaderManager->setFloatValue("textureBlend", blendFactor);
	}
}
/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.      ***/
	/******************************************************************/
	// Rendering the Lighthouse
	// set the XYZ scale for the mesh
	// Lighthouse Base
	scaleXYZ = glm::vec3(5.0f, 15.0f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 7.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, -10.0f, -10.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("walls"); // texture instead of color
	SetShaderMaterial("walls");

	// draw the mesh with transformation values
	m_basicMeshes->DrawTaperedCylinderMesh();
	/****************************************************************/

	// set the XYZ scale for the mesh
	// Lighthouse Light
	scaleXYZ = glm::vec3(1.5f, 3.0f, 1.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 5.0f, -10.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.4, 0.4, 0.4, 1.0);

	// draw the mesh with transformation values
	m_basicMeshes->DrawTaperedCylinderMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Lighthouse Light Glass
	scaleXYZ = glm::vec3(4.3f, 17.6f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 7.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, -10.0f, -10.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(1.0, 1.0, 1.0, 0.1);

	// draw the mesh with transformation values
	m_basicMeshes->DrawTaperedCylinderMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Lighthouse Top
	scaleXYZ = glm::vec3(2.4f, 2.8f, 1.8f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 10.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 7.5f, -8.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 0.4, 0.5, 0.9);
	SetShaderTexture("roof"); // texture instead of color
	SetShaderMaterial("roof");

	// draw the mesh with transformation values
	m_basicMeshes->DrawConeMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Lighthouse house
	scaleXYZ = glm::vec3(10.0, 5.0f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -7.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.3f, -7.5f, -10.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("walls"); // texture instead of color
	SetShaderMaterial("walls");

	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/

	// set the XYZ scale for the mesh
	// Lighthouse House Roof
	scaleXYZ = glm::vec3(2.0f, 7.0f, 2.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -22.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(6.0f, -4.5f, -10.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 0.4, 0.5, 1.0);
	SetShaderTexture("roof"); // texture instead of color
	SetShaderMaterial("roof");

	// draw the mesh with transformation values
	m_basicMeshes->DrawPrismMesh();

	/****************************************************************/
		// set the XYZ scale for the mesh
	// Lighthouse house chimney 
	scaleXYZ = glm::vec3(1.0, 9.0f, 1.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -7.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(10.0f, -6.0f, -9.2f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("walls"); // texture instead of color
	SetShaderMaterial("walls");

	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Lighthouse house 2
	scaleXYZ = glm::vec3(6.5, 5.0f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -7.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(16.0f, -9.5f, -8.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("walls"); // texture instead of color
	SetShaderMaterial("walls");

	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/

	// set the XYZ scale for the mesh
	// Lighthouse House Roof
	scaleXYZ = glm::vec3(2.0f, 7.0f, 2.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -22.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(15.5f, -6.2f, -7.8f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 0.4, 0.5, 1.0);
	SetShaderTexture("roof2"); // texture instead of color
	SetShaderMaterial("roof");

	// draw the mesh with transformation values
	m_basicMeshes->DrawPrismMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Lighthouse Utility(?) Box
	scaleXYZ = glm::vec3(2.0f, 2.0f, 2.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -7.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(22.0f, -9.5f, -8.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("walls"); // texture instead of color
	SetShaderMaterial("walls");

	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/
		// set the XYZ scale for the mesh
	// Lighthouse house 3
	scaleXYZ = glm::vec3(6.5f, 5.5f, 2.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -7.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(30.2f, -9.5f, -8.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("walls"); // texture instead of color
	SetShaderMaterial("walls");

	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/

	// set the XYZ scale for the mesh
	// Lighthouse House Roof 3
	scaleXYZ = glm::vec3(2.0f, 7.0f, 2.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -22.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(29.5f, -6.2f, -7.8f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 0.4, 0.5, 1.0);
	SetShaderTexture("roof2"); // texture instead of color
	SetShaderMaterial("roof");

	// draw the mesh with transformation values
	m_basicMeshes->DrawPrismMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// moss rocks
	scaleXYZ = glm::vec3(35.0f, 10.0f, 20.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, -20.0f, -15.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("moss"); // texture instead of color
	SetShaderMaterial("floor");

	// draw the mesh with transformation values
	m_basicMeshes->DrawTaperedCylinderMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// moss rocks
	scaleXYZ = glm::vec3(35.0f, 10.0f, 20.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(20.0f, -20.0f, -13.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("moss"); // texture instead of color
	SetShaderMaterial("floor");

	// draw the mesh with transformation values
	m_basicMeshes->DrawTaperedCylinderMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Boulder 1 base
	scaleXYZ = glm::vec3(10.0f, 17.0f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -7.5f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-13.0f, -35.0f, 0.75f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("boulder"); // texture instead of color
	SetShaderMaterial("floor");

	// draw the mesh with transformation values
	m_basicMeshes->DrawTaperedCylinderMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Boulder 1 top
	scaleXYZ = glm::vec3(5.5f, 7.5f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-12.5f, -20.0f, 1.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("boulder"); // texture instead of color
	SetShaderMaterial("floor");

	// draw the mesh with transformation values
	m_basicMeshes->DrawSphereMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Boulder 2 base
	scaleXYZ = glm::vec3(12.0f, 17.0f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -7.5f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-25.0f, -27.0f, -5.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("boulder"); // texture instead of color
	SetShaderMaterial("floor");

	// draw the mesh with transformation values
	m_basicMeshes->DrawTaperedCylinderMesh();
	/****************************************************************/
	// set the XYZ scale for the mesh
	// Boulder 2 top
	scaleXYZ = glm::vec3(5.5f, 7.5f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-25.0f, -12.0f, -5.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(1.0, 1.0, 1.0, 1.0);
	SetShaderTexture("boulder"); // texture instead of color
	SetShaderMaterial("floor");

	// draw the mesh with transformation values
	m_basicMeshes->DrawSphereMesh();
}