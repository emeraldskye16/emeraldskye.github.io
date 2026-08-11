// TextureManager.cpp
#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

TextureManager::TextureManager()
{
    m_loadedTextures = 0;
    for (int i = 0; i < 16; ++i)
    {
        m_textureIDs[i].tag = "";
        m_textureIDs[i].ID = 0;
    }
}

TextureManager::~TextureManager()
{
    DestroyGLTextures();
}

bool TextureManager::CreateGLTexture(const char* filename, const std::string& tag)
{
    // Resolve path more robustly and print diagnostics
    fs::path requested(filename);
    fs::path resolved = requested;

    // Try as given, then relative to current working dir, then try common alternatives
    if (!fs::exists(resolved))
    {
        resolved = fs::current_path() / requested;
        if (!fs::exists(resolved))
        {
            // Try parent directories (useful when running from Debug)
            resolved = fs::current_path() / ".." / ".." / requested;
            if (!fs::exists(resolved))
            {
                resolved = requested; // keep original for message
            }
        }
    }

    std::cout << "[TextureManager] Request: \"" << filename << "\" -> Resolved: \"" << resolved.string()
              << "\" exists? " << (fs::exists(resolved) ? "yes" : "no") << std::endl;

    if (!fs::exists(resolved))
    {
        std::cout << "[TextureManager] Could not find texture file: " << filename << std::endl;
        return false;
    }

    int width = 0, height = 0, channels = 0;
    GLuint textureID = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(resolved.string().c_str(), &width, &height, &channels, 0);

    if (!image)
    {
        std::cout << "[TextureManager] stbi_load failed for: " << resolved.string() << std::endl;
        return false;
    }

    std::cout << "[TextureManager] Loaded image: " << resolved.string()
        << " w:" << width << " h:" << height
        << " c:" << channels << std::endl;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Ensure correct alignment for tightly packed images (prevents black uploads)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    else
    {
        std::cout << "[TextureManager] Unsupported channel count: " << channels << " for " << resolved.string() << std::endl;
        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (m_loadedTextures >= 16)
    {
        std::cout << "[TextureManager] Maximum texture slots reached. Cannot add: " << tag << std::endl;
        glDeleteTextures(1, &textureID);
        return false;
    }

    m_textureIDs[m_loadedTextures].ID = textureID;
    m_textureIDs[m_loadedTextures].tag = tag;

    std::cout << "[TextureManager] Assigned slot=" << m_loadedTextures
              << " GLID=" << textureID << " tag=\"" << tag << "\"" << std::endl;

    ++m_loadedTextures;

    return true;
}

void TextureManager::BindGLTextures()
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
        std::cout << "[TextureManager] Bound slot=" << i << " GLID=" << m_textureIDs[i].ID << " tag=\"" << m_textureIDs[i].tag << "\"" << std::endl;
    }
}

void TextureManager::DestroyGLTextures()
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        if (m_textureIDs[i].ID != 0)
        {
            glDeleteTextures(1, &m_textureIDs[i].ID);
            m_textureIDs[i].ID = 0;
        }
    }
    m_loadedTextures = 0;
}

int TextureManager::FindTextureID(const std::string& tag) const
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        if (m_textureIDs[i].tag == tag)
            return m_textureIDs[i].ID;
    }
    return -1;
}

int TextureManager::FindTextureSlot(const std::string& tag) const
{
    for (int i = 0; i < m_loadedTextures; ++i)
    {
        if (m_textureIDs[i].tag == tag)
            return i;
    }
    return -1;
}

void TextureManager::LoadSceneTextures()
{
    bool ok = false;

    // Use three levels up from Debug so path matches the shaders resolution used in MainCode
    ok = CreateGLTexture("../../../Utilities/textures/dirt.jpg", "floor");
    ok = CreateGLTexture("../../../Utilities/textures/dirtydrywall.jpg", "walls");
    ok = CreateGLTexture("../../../Utilities/textures/red.jpg", "roof");
    ok = CreateGLTexture("../../../Utilities/textures/roof2.jpg", "roof2");
    ok = CreateGLTexture("../../../Utilities/textures/boulder.jpg", "boulder");
    ok = CreateGLTexture("../../../Utilities/textures/mossyrock.jpg", "moss");

    BindGLTextures();
}