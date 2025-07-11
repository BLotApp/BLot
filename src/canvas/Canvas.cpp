#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Canvas.h"
#include "rendering/Graphics.h"
#include "rendering/Blend2DRenderer.h"
#include <filesystem>

struct Canvas::Impl {
	GLuint framebuffer = 0;
	GLuint colorTexture = 0;
	GLuint depthRenderbuffer = 0;
	GLuint shaderProgram = 0;
	GLuint VAO = 0;
	GLuint VBO = 0;
};

Canvas::Canvas(int width, int height, std::shared_ptr<Graphics> graphics)
    : m_width(width)
    , m_height(height)
    , m_impl(std::make_unique<Impl>())
    , m_graphics(graphics)
    , m_hasFill(true)
    , m_hasStroke(true)
    , m_fillColor(1.0f, 1.0f, 1.0f, 1.0f)
    , m_strokeColor(0.0f, 0.0f, 0.0f, 1.0f)
    , m_strokeWeight(1.0f)
    , m_textSize(12.0f)
    , m_textAlign(0)
    , m_time(0.0f)
    , m_frameRate(60.0f)
    , m_frameCount(0)
{
    m_currentMatrix = glm::mat4(1.0f);
    m_graphics->setCanvasSize(m_width, m_height);
    initFramebuffer();
    initShaders();
    // Set default background to white
    clear(1.0f, 1.0f, 1.0f, 1.0f);
}

Canvas::~Canvas() {
	if (m_impl->framebuffer) {
		glDeleteFramebuffers(1, &m_impl->framebuffer);
	}
	if (m_impl->colorTexture) {
		glDeleteTextures(1, &m_impl->colorTexture);
	}
	if (m_impl->depthRenderbuffer) {
		glDeleteRenderbuffers(1, &m_impl->depthRenderbuffer);
	}
	if (m_impl->shaderProgram) {
		glDeleteProgram(m_impl->shaderProgram);
	}
	if (m_impl->VAO) {
		glDeleteVertexArrays(1, &m_impl->VAO);
	}
	if (m_impl->VBO) {
		glDeleteBuffers(1, &m_impl->VBO);
	}
}

void Canvas::resize(int width, int height) {
	m_width = width;
	m_height = height;
	m_graphics->setCanvasSize(m_width, m_height);
	initFramebuffer();
	// Set default background to white after resize
	clear(1.0f, 1.0f, 1.0f, 1.0f);
}

void Canvas::clear() {
	clear(1.0f, 1.0f, 1.0f, 1.0f);
}

void Canvas::clear(float r, float g, float b, float a) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_impl->framebuffer);
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::background(float r, float g, float b, float a) {
	clear(r, g, b, a);
}

void Canvas::fill(float r, float g, float b, float a) {
	m_fillColor = glm::vec4(r, g, b, a);
	m_hasFill = true;
}

void Canvas::noFill() {
	m_hasFill = false;
}

void Canvas::stroke(float r, float g, float b, float a) {
	m_strokeColor = glm::vec4(r, g, b, a);
	m_hasStroke = true;
}

void Canvas::noStroke() {
	m_hasStroke = false;
}

void Canvas::strokeWeight(float weight) {
	m_strokeWeight = weight;
}

void Canvas::strokeCap(int cap) {
	if (m_graphics) m_graphics->setStrokeCap(cap);
}
void Canvas::strokeJoin(int join) {
	if (m_graphics) m_graphics->setStrokeJoin(join);
}
void Canvas::strokeDash(const std::vector<float>& dashes, float offset) {
	if (m_graphics) m_graphics->setStrokeDash(dashes, offset);
}

void Canvas::rect(float x, float y, float width, float height) {
	if (m_graphics) {
		m_graphics->setFillColor(m_fillColor.r, m_fillColor.g, m_fillColor.b, m_fillColor.a);
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g, m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawRect(x, y, width, height);
	}
}

void Canvas::ellipse(float x, float y, float width, float height) {
	if (m_graphics) {
		m_graphics->setFillColor(m_fillColor.r, m_fillColor.g, m_fillColor.b, m_fillColor.a);
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g, m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawEllipse(x, y, width, height);
	}
}

void Canvas::line(float x1, float y1, float x2, float y2) {
	if (m_graphics) {
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g, m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawLine(x1, y1, x2, y2);
	}
}

void Canvas::triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	if (m_graphics) {
		m_graphics->setFillColor(m_fillColor.r, m_fillColor.g, m_fillColor.b, m_fillColor.a);
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g, m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawTriangle(x1, y1, x2, y2, x3, y3);
	}
}

void Canvas::circle(float x, float y, float diameter) {
	ellipse(x, y, diameter, diameter);
}

void Canvas::text(const std::string& text, float x, float y) {
	if (m_graphics) {
		m_graphics->setFont("Arial", m_textSize);
		m_graphics->drawText(text, x, y);
	}
}

void Canvas::textSize(float size) {
	m_textSize = size;
}

void Canvas::textAlign(int align) {
	m_textAlign = align;
}

void Canvas::pushMatrix() {
	m_matrixStack.push_back(m_currentMatrix);
}

void Canvas::popMatrix() {
	if (!m_matrixStack.empty()) {
		m_currentMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
	}
}

void Canvas::translate(float x, float y) {
	m_currentMatrix = glm::translate(m_currentMatrix, glm::vec3(x, y, 0.0f));
}

void Canvas::rotate(float angle) {
	m_currentMatrix = glm::rotate(m_currentMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Canvas::scale(float x, float y) {
	m_currentMatrix = glm::scale(m_currentMatrix, glm::vec3(x, y, 1.0f));
}

void Canvas::update(float deltaTime) {
	m_time += deltaTime;
	m_frameCount++;
}

void Canvas::render() {
    // Shape rendering is now handled by ECS system
    if (m_graphics) {
        // Clear the canvas
        m_graphics->clear(0.0f, 0.0f, 0.0f, 0.0f);
    }
    // Upload Blend2D image to OpenGL texture
    Blend2DRenderer* renderer = dynamic_cast<Blend2DRenderer*>(m_graphics->getRenderer());
    if (renderer) {
        const BLImage& img = renderer->getImage();
        BLImageData imgData;
        if (img.getData(&imgData) == BL_SUCCESS) {
            glBindTexture(GL_TEXTURE_2D, m_impl->colorTexture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.width(), img.height(), GL_BGRA, GL_UNSIGNED_BYTE, imgData.pixelData);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
		
        // Debug: Save the image after drawing
        renderer->flush();
        img.writeToFile("debug_after_render.png");
    }
}

void Canvas::saveFrame(const std::string& filename) {
    if (m_graphics) {
        Blend2DRenderer* renderer = dynamic_cast<Blend2DRenderer*>(m_graphics->getRenderer());
        if (renderer) {
            renderer->flush();
            const BLImage& img = renderer->getImage();
            printf("[Canvas] Image info: %p size: %d x %d, format: %u, empty: %d\n", (void*)&img, img.width(), img.height(), img.format(), img.empty());
            std::string outFile = filename;
            if (std::filesystem::path(outFile).extension().empty()) {
                outFile += ".png";
            }
            BLResult res = img.writeToFile(outFile.c_str());
            if (res == BL_SUCCESS) {
                printf("[Canvas] Saved Blend2D image to: %s\n", outFile.c_str());
            } else {
                printf("[Canvas] ERROR: Failed to save Blend2D image to: %s (error code: %d)\n", outFile.c_str(), res);
            }
        } else {
            printf("[Canvas] ERROR: Renderer is not Blend2DRenderer.\n");
        }
    } else {
        printf("[Canvas] ERROR: m_graphics is null.\n");
    }
}

void Canvas::exportSVG(const std::string& filename) {
	// Implementation for exporting as SVG
	(void)filename;
}

unsigned int Canvas::getColorTexture() const {
    return m_impl->colorTexture;
}

void Canvas::initFramebuffer() {
	// Create framebuffer for off-screen rendering
	glGenFramebuffers(1, &m_impl->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_impl->framebuffer);
	
	// Create color texture
	glGenTextures(1, &m_impl->colorTexture);
	glBindTexture(GL_TEXTURE_2D, m_impl->colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_impl->colorTexture, 0);
	
	// Create depth renderbuffer
	glGenRenderbuffers(1, &m_impl->depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_impl->depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_impl->depthRenderbuffer);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// Handle framebuffer error
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::initShaders() {
	// Basic shader for rendering
	const char* vertexShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec2 aTexCoord;
		
		out vec2 TexCoord;
		uniform mat4 model;
		uniform mat4 projection;
		
		void main() {
			gl_Position = projection * model * vec4(aPos, 1.0);
			TexCoord = aTexCoord;
		}
	)";
	
	const char* fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;
		in vec2 TexCoord;
		
		uniform sampler2D texture1;
		
		void main() {
			FragColor = texture(texture1, TexCoord);
		}
	)";
	
	// Compile and link shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	m_impl->shaderProgram = glCreateProgram();
	glAttachShader(m_impl->shaderProgram, vertexShader);
	glAttachShader(m_impl->shaderProgram, fragmentShader);
	glLinkProgram(m_impl->shaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	// Set up vertex data
	glGenVertexArrays(1, &m_impl->VAO);
	glGenBuffers(1, &m_impl->VBO);
	
	glBindVertexArray(m_impl->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_impl->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
	glBindVertexArray(0);
} 