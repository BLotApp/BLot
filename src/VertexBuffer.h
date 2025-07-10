#pragma once
#include <vector>
#include <cstdint>

class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	void setData(const std::vector<float>& data, int componentsPerVertex);
	void bind() const;
	void unbind() const;
	void draw(int mode, int count) const;

private:
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	int m_components = 0;
}; 