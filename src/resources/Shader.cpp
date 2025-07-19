#include "resources/Shader.h"
#include <glad/gl.h>

Shader::Shader() {}
Shader::~Shader() {}

bool Shader::load(const std::string &vertSrc, const std::string &fragSrc) {
	// Stub: always return true
	return true;
}

void Shader::use() {
	// Stub: do nothing
}

void Shader::setUniform(const std::string &name, float value) {}
void Shader::setUniform(const std::string &name, int value) {}
void Shader::setUniform(const std::string &name, const glm::vec2 &value) {}
void Shader::setUniform(const std::string &name, const glm::vec3 &value) {}
void Shader::setUniform(const std::string &name, const glm::vec4 &value) {}
void Shader::setUniform(const std::string &name, const glm::mat4 &value) {}

unsigned int Shader::getProgram() const { return m_program; }
