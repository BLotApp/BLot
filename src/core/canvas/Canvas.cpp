#include "core/canvas/Canvas.h"

#include "rendering/gladGlfw.h"

#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>
#include <spdlog/spdlog.h>

#include "core/AddonManager.h"
#include "core/BlotEngine.h"
#include "core/ISettings.h"
#include "core/json.h"
#include "ecs/ECSManager.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include "ecs/components/TransformComponent.h"
#include "rendering/Graphics.h"
#include "rendering/IRenderer.h"

namespace blot {

struct Canvas::Impl {
	GLuint framebuffer = 0;
	GLuint colorTexture = 0;
	GLuint depthRenderbuffer = 0;
	GLuint shaderProgram = 0;
	GLuint VAO = 0;
	GLuint VBO = 0;
};

Canvas::Canvas(const CanvasSettings &settings, BlotEngine *engine)
	: m_width(settings.width), m_height(settings.height), m_settings(settings),
	  m_impl(std::make_unique<Impl>()),
	  m_graphics(std::make_shared<Graphics>()), m_engine(engine),
	  m_hasFill(true), m_hasStroke(true),
	  m_fillColor(settings.r, settings.g, settings.b, settings.a),
	  m_strokeColor(0, 0, 0, 1), m_strokeWeight(1.0f), m_textSize(12.0f),
	  m_textAlign(0), m_time(0.0f), m_frameRate(60.0f), m_frameCount(0) {
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

void Canvas::clear() { clear(1.0f, 1.0f, 1.0f, 1.0f); }

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

void Canvas::noFill() { m_hasFill = false; }

void Canvas::stroke(float r, float g, float b, float a) {
	m_strokeColor = glm::vec4(r, g, b, a);
	m_hasStroke = true;
}

void Canvas::noStroke() { m_hasStroke = false; }

void Canvas::strokeWeight(float weight) { m_strokeWeight = weight; }

void Canvas::strokeCap(int cap) {
	if (m_graphics)
		m_graphics->setStrokeCap(cap);
}
void Canvas::strokeJoin(int join) {
	if (m_graphics)
		m_graphics->setStrokeJoin(join);
}
void Canvas::strokeDash(const std::vector<float> &dashes, float offset) {
	if (m_graphics)
		m_graphics->setStrokeDash(dashes, offset);
}

void Canvas::rect(float x, float y, float width, float height) {
	if (m_graphics) {
		m_graphics->setFillColor(m_fillColor.r, m_fillColor.g, m_fillColor.b,
								 m_fillColor.a);
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g,
								   m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawRect(x, y, width, height);
	}
}

void Canvas::ellipse(float x, float y, float width, float height) {
	if (m_graphics) {
		m_graphics->setFillColor(m_fillColor.r, m_fillColor.g, m_fillColor.b,
								 m_fillColor.a);
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g,
								   m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawEllipse(x, y, width, height);
	}
}

void Canvas::line(float x1, float y1, float x2, float y2) {
	if (m_graphics) {
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g,
								   m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawLine(x1, y1, x2, y2);
	}
}

void Canvas::triangle(float x1, float y1, float x2, float y2, float x3,
					  float y3) {
	if (m_graphics) {
		m_graphics->setFillColor(m_fillColor.r, m_fillColor.g, m_fillColor.b,
								 m_fillColor.a);
		m_graphics->setStrokeColor(m_strokeColor.r, m_strokeColor.g,
								   m_strokeColor.b, m_strokeColor.a);
		m_graphics->setStrokeWidth(m_strokeWeight);
		m_graphics->drawTriangle(x1, y1, x2, y2, x3, y3);
	}
}

void Canvas::circle(float x, float y, float diameter) {
	ellipse(x, y, diameter, diameter);
}

void Canvas::text(const std::string &text, float x, float y) {
	if (m_graphics) {
		m_graphics->setFont("Arial", m_textSize);
		m_graphics->drawText(text, x, y);
	}
}

void Canvas::textSize(float size) { m_textSize = size; }

void Canvas::textAlign(int align) { m_textAlign = align; }

void Canvas::pushMatrix() { m_matrixStack.push_back(m_currentMatrix); }

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
	m_currentMatrix =
		glm::rotate(m_currentMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
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
		// Clear the canvas with white background
		m_graphics->clear(1.0f, 1.0f, 1.0f, 1.0f);

		// Render ECS shapes
		renderECSShapes();
	}
	// Remove Blend2D-specific image upload and BLImage logic from core
}

void Canvas::renderECSShapes() {
	if (!m_ecs || !m_graphics) {
		spdlog::debug(
			"[Canvas] renderECSShapes: m_ecs=0x{:X}, m_graphics=0x{:X}",
			reinterpret_cast<uintptr_t>(m_ecs),
			reinterpret_cast<uintptr_t>(m_graphics.get()));
		return;
	}

	// Get the renderer from graphics
	auto renderer = m_graphics->getRenderer();
	if (!renderer) {
		spdlog::error("[Canvas] renderECSShapes: No renderer found");
		return;
	}

	// Get all entities with Transform, Shape, and Style components
	auto view = m_ecs->view<blot::components::Transform,
							blot::components::Shape, blot::components::Style>();

	spdlog::debug(
		"[Canvas] renderECSShapes: Checking for entities with shapes...");
	spdlog::debug("[Canvas] Total entities in registry: {}",
				  m_ecs->getEntityCount());

	// Debug: Check what components each entity has
	auto allEntities = m_ecs->getAllEntities();
	spdlog::debug("[Canvas] All entities: {}", allEntities.size());
	for (auto entity : allEntities) {
		bool hasTransform =
			m_ecs->hasComponent<blot::components::Transform>(entity);
		bool hasShape = m_ecs->hasComponent<blot::components::Shape>(entity);
		bool hasStyle = m_ecs->hasComponent<blot::components::Style>(entity);
		spdlog::debug("[Canvas] Entity {}: Transform={}, Shape={}, Style={}",
					  (unsigned int)entity, hasTransform, hasShape, hasStyle);
	}

	for (auto entity : view) {
		auto &transform = view.get<blot::components::Transform>(entity);
		auto &shape = view.get<blot::components::Shape>(entity);
		auto &style = view.get<blot::components::Style>(entity);

		// Set fill and stroke colors
		if (style.hasFill) {
			m_graphics->setFillColor(style.fillR, style.fillG, style.fillB,
									 style.fillA);
		}
		if (style.hasStroke) {
			m_graphics->setStrokeColor(style.strokeR, style.strokeG,
									   style.strokeB, style.strokeA);
			m_graphics->setStrokeWidth(style.strokeWidth);
		}

		// Calculate position with transform
		float x = transform.position.x + shape.x1;
		float y = transform.position.y + shape.y1;
		float width = shape.x2 - shape.x1;
		float height = shape.y2 - shape.y1;

		// Apply transform scaling
		x *= transform.scale.x;
		y *= transform.scale.y;
		width *= transform.scale.x;
		height *= transform.scale.y;

		// Debug: Print raw shape coordinates
		spdlog::debug("[Canvas] Raw shape coords: x1={:.2f}, y1={:.2f}, "
					  "x2={:.2f}, y2={:.2f}",
					  shape.x1, shape.y1, shape.x2, shape.y2);
		spdlog::debug(
			"[Canvas] Transform: x={:.2f}, y={:.2f}, scale=(%.2f,%.2f)",
			transform.position.x, transform.position.y, transform.scale.x,
			transform.scale.y);

		// Render based on shape type
		spdlog::debug("[Canvas] Rendering shape: type={}, x={:.2f}, y={:.2f}, "
					  "w={:.2f}, h={:.2f}, hasFill={}, hasStroke={}",
					  (int)shape.type, x, y, width, height, style.hasFill,
					  style.hasStroke);

		switch (shape.type) {
		case blot::components::Shape::Type::Rectangle:
			if (style.hasFill) {
				spdlog::debug("[Canvas] Drawing filled rectangle");
				m_graphics->drawRect(x, y, width, height);
			}
			if (style.hasStroke) {
				spdlog::debug("[Canvas] Drawing stroked rectangle");
				m_graphics->drawRect(x, y, width, height);
			}
			break;

		case blot::components::Shape::Type::Ellipse:
			if (style.hasFill)
				m_graphics->drawEllipse(x + width * 0.5f, y + height * 0.5f,
										width * 0.5f, height * 0.5f);
			if (style.hasStroke)
				m_graphics->drawEllipse(x + width * 0.5f, y + height * 0.5f,
										width * 0.5f, height * 0.5f);
			break;

		case blot::components::Shape::Type::Line:
			if (style.hasStroke) {
				float x2 = transform.position.x + shape.x2;
				float y2 = transform.position.y + shape.y2;
				x2 *= transform.scale.x;
				y2 *= transform.scale.y;
				m_graphics->drawLine(x, y, x2, y2);
			}
			break;

		case blot::components::Shape::Type::Polygon:
			// For now, render as circle - can be enhanced later
			if (style.hasFill)
				m_graphics->drawEllipse(x + width * 0.5f, y + height * 0.5f,
										width * 0.5f, height * 0.5f);
			if (style.hasStroke)
				m_graphics->drawEllipse(x + width * 0.5f, y + height * 0.5f,
										width * 0.5f, height * 0.5f);
			break;

		case blot::components::Shape::Type::Star:
			// For now, render as circle - can be enhanced later
			if (style.hasFill)
				m_graphics->drawEllipse(x + width * 0.5f, y + height * 0.5f,
										width * 0.5f, height * 0.5f);
			if (style.hasStroke)
				m_graphics->drawEllipse(x + width * 0.5f, y + height * 0.5f,
										width * 0.5f, height * 0.5f);
			break;
		}
	}
}

void Canvas::saveFrame(const std::string &filename) {
	// Remove Blend2D-specific image saving from core
	spdlog::warn("[Canvas] saveFrame: Blend2D-specific logic moved to addon.");
}

void Canvas::exportSVG(const std::string &filename) {
	// Implementation for exporting as SVG
	(void)filename;
}

unsigned int Canvas::getColorTexture() const { return m_impl->colorTexture; }

void Canvas::initFramebuffer() {
	// Create framebuffer for off-screen rendering
	glGenFramebuffers(1, &m_impl->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_impl->framebuffer);

	// Create color texture
	glGenTextures(1, &m_impl->colorTexture);
	glBindTexture(GL_TEXTURE_2D, m_impl->colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   m_impl->colorTexture, 0);

	// Create depth renderbuffer
	glGenRenderbuffers(1, &m_impl->depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_impl->depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width,
						  m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
							  GL_RENDERBUFFER, m_impl->depthRenderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// Handle framebuffer error
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::initShaders() {
	// Basic shader for rendering
	const char *vertexShaderSource = R"(
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

	const char *fragmentShaderSource = R"(
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
						  (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
						  (void *)(3 * sizeof(float)));

	glBindVertexArray(0);
}

void Canvas::switchRenderer(RendererType type) {
	auto *addonManager = m_engine->getAddonManager();
	// Remove or replace the line: auto newRenderer =
	// addonManager->createRenderer(type); The original code had this line, but
	// the user's edit hint implies its removal. Assuming the user's intent is
	// to remove this line. If the user intended to replace it with a new
	// renderer, they would need to provide the new renderer object. For now,
	// we'll just log an error if the renderer type is not found.
	spdlog::error("switchRenderer: Renderer type {} not found or implemented.",
				  static_cast<int>(type));
}

void Canvas::setRenderer(std::unique_ptr<IRenderer> renderer) {
	if (renderer) {
		// Initialize the renderer with current canvas dimensions
		if (renderer->initialize(m_width, m_height)) {
			// Set the new renderer in graphics
			m_graphics->setRenderer(renderer.get());
			spdlog::info("Set canvas renderer to: {}", renderer->getName());
		} else {
			spdlog::error("Failed to initialize renderer: {}",
						  renderer->getName());
		}
	}
}

RendererType Canvas::getRendererType() const {
	if (m_graphics && m_graphics->getRenderer()) {
		return m_graphics->getRenderer()->getType();
	}
	return RendererType::Blend2D; // Default or fallback
}

json Canvas::getSettings() const {
	json j;
	j["width"] = m_width;
	j["height"] = m_height;
	j["name"] = m_name;
	j["background"] = {m_settings.r, m_settings.g, m_settings.b, m_settings.a};
	j["samples"] = m_settings.samples;
	return j;
}

void Canvas::setSettings(const json &settings) {
	if (settings.contains("width"))
		m_width = settings["width"].get<int>();
	if (settings.contains("height"))
		m_height = settings["height"].get<int>();
	if (settings.contains("name"))
		m_name = settings["name"].get<std::string>();
	if (settings.contains("background")) {
		auto bg = settings["background"];
		if (bg.is_array() && bg.size() == 4) {
			m_settings.r = bg[0].get<float>();
			m_settings.g = bg[1].get<float>();
			m_settings.b = bg[2].get<float>();
			m_settings.a = bg[3].get<float>();
		}
	}
	if (settings.contains("samples"))
		m_settings.samples = settings["samples"].get<int>();
	resize(m_width, m_height);
}

} // namespace blot
