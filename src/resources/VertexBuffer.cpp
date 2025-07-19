#include "resources/VertexBuffer.h"
#include <glad/gl.h>

VertexBuffer::VertexBuffer() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

void VertexBuffer::setData(const std::vector<float> &data,
						   int componentsPerVertex) {
	m_components = componentsPerVertex;
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(),
				 GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, m_components, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glBindVertexArray(0);
}

void VertexBuffer::bind() const { glBindVertexArray(m_vao); }

void VertexBuffer::unbind() const { glBindVertexArray(0); }

void VertexBuffer::draw(int mode, int count) const {
	glBindVertexArray(m_vao);
	glDrawArrays(mode, 0, count);
	glBindVertexArray(0);
}
