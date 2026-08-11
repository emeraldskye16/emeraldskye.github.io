#include "ShaderManager.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

ShaderManager::ShaderManager()
    : m_programID(0)
{
}

ShaderManager::~ShaderManager()
{
    if (m_programID != 0)
        glDeleteProgram(m_programID);
}

std::string ShaderManager::ReadFile(const std::string& path) const
{
    std::cout << "Trying to open: " << path << "\n";
    std::cout << "Exists? " << fs::exists(fs::path(path)) << "\n";

    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "Failed to open shader file: " << path << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderManager::CompileShader(GLenum type, const std::string& source) const
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error: " << infoLog << "\n";
    }

    return shader;
}

GLuint ShaderManager::LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    std::string vertexCode = ReadFile(vertex_file_path);
    std::string fragmentCode = ReadFile(fragment_file_path);

    if (vertexCode.empty() || fragmentCode.empty())
        return 0;

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);

    GLint success = 0;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        std::cerr << "Program link error: " << infoLog << "\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_programID);
        m_programID = 0;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return m_programID;
}

void ShaderManager::use()
{
    glUseProgram(m_programID);
}

void ShaderManager::setMat4Value(const std::string& name, const glm::mat4& value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
}

void ShaderManager::setVec4Value(const std::string& name, const glm::vec4& value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniform4fv(loc, 1, &value[0]);
}

void ShaderManager::setVec3Value(const std::string& name, float x, float y, float z) const
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniform3f(loc, x, y, z);
}

void ShaderManager::setIntValue(const std::string& name, int value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniform1i(loc, value);
}

void ShaderManager::setFloatValue(const std::string& name, float value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniform1f(loc, value);
}

void ShaderManager::setBoolValue(const std::string& name, bool value) const
{
    setIntValue(name, value ? 1 : 0);
}
