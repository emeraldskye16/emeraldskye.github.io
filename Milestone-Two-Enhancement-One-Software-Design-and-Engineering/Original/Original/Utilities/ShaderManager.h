#pragma once

#include <GL/glew.h>        // GLEW library

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	unsigned int m_programID;

	// Read shader file into string
	std::string ReadFile(const std::string& path) const;

	// Load and link shaders, returns program ID (0 on failure)
	GLuint LoadShaders(
		const char* vertex_file_path,
		const char* fragment_file_path);

	// activate the shader
	// ------------------------------------------------------------------------
	void use();

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBoolValue(const std::string &name, bool value) const;

	// ------------------------------------------------------------------------
	void setIntValue(const std::string &name, int value) const;

	// ------------------------------------------------------------------------
	void setFloatValue(const std::string &name, float value) const;

	// ------------------------------------------------------------------------
	inline void setVec2Value(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
	}

	inline void setVec2Value(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(m_programID, name.c_str()), x, y);
	}

	// ------------------------------------------------------------------------
	inline void setVec3Value(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
	}
	// float overload implemented in .cpp
	void setVec3Value(const std::string &name, float x, float y, float z) const;

	// ------------------------------------------------------------------------
	// vec4 overload taking glm::vec4 implemented in .cpp
	void setVec4Value(const std::string &name, const glm::vec4 &value) const;
	inline void setVec4Value(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(m_programID, name.c_str()), x, y, z, w);
	}

	// ------------------------------------------------------------------------
	inline void setMat2Value(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	// ------------------------------------------------------------------------
	inline void setMat3Value(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	// ------------------------------------------------------------------------
	// mat4 implemented in .cpp
	void setMat4Value(const std::string &name, const glm::mat4 &mat) const;

	// ------------------------------------------------------------------------
	inline void setSampler2DValue(const std::string& name, const int &value) const
	{
		glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
	}

	// Compile single shader from source
	GLuint CompileShader(GLenum type, const std::string& source) const;
};