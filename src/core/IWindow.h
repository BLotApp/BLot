#pragma once

#include <string>

namespace blot {

enum class WindowFlags {
	None = 0,
	Resizable = 1 << 0,
	Movable = 1 << 1,
	Closable = 1 << 2,
	Minimizable = 1 << 3,
	Maximizable = 1 << 4
};

class IWindow {
  public:
	virtual ~IWindow() = default;

	// Window management
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void close() = 0;
	virtual bool isVisible() const = 0;

	// Window properties
	virtual void setTitle(const std::string &title) = 0;
	virtual std::string getTitle() const = 0;
	virtual void setFlags(WindowFlags flags) = 0;
	virtual WindowFlags getFlags() const = 0;

	// Window state
	virtual void setPosition(int x, int y) = 0;
	virtual void getPosition(int &x, int &y) const = 0;
	virtual void setSize(int width, int height) = 0;
	virtual void getSize(int &width, int &height) const = 0;

	// Rendering
	virtual void renderContents() = 0;
};

} // namespace blot
