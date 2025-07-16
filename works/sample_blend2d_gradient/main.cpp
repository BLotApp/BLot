#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <spdlog/spdlog.h>
#include "work_entry.h" // for createWork()
#include "framework/IWork.h"

int main() {
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int windowWidth = 1280, windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Blot Work", nullptr, nullptr);
    if (!window) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoaderLoadGL()) {
        spdlog::error("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glfwSwapInterval(1);

    std::unique_ptr<IWork> work = createWork();
    work->setup(nullptr); // No FrameworkApp, pass nullptr or adapt as needed

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        work->update(0.0f); // Pass deltaTime if you want
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        work->draw();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
} 