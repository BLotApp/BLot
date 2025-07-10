#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
	Shader();
	~Shader();

	bool load(const std::string& vertSrc, const std::string& fragSrc);
	void use();
	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, int value);
	void setUniform(const std::string& name, const glm::vec2& value);
	void setUniform(const std::string& name, const glm::vec3& value);
	void setUniform(const std::string& name, const glm::vec4& value);
	void setUniform(const std::string& name, const glm::mat4& value);

	unsigned int getProgram() const;

private:
	unsigned int m_program = 0;
}; 