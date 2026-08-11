// TextureManager.h
#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <GL/glew.h>

class TextureManager
{
public:
    struct TEXTURE_INFO
    {
        std::string tag;
        uint32_t ID;
    };

    TextureManager();
    ~TextureManager();

    bool CreateGLTexture(const char* filename, const std::string& tag);
    void BindGLTextures();
    void DestroyGLTextures();

    int  FindTextureID(const std::string& tag) const;
    int  FindTextureSlot(const std::string& tag) const;

    void LoadSceneTextures();   // same textures as original CS330 project

private:
    TEXTURE_INFO m_textureIDs[16];
    int          m_loadedTextures;
};