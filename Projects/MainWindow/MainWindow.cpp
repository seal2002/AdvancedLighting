#include "MainWindow.hpp"

#include <GLFW/glfw3.h>

namespace OpenGLWindow
{
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    bool keyPressed[GLFW_KEY_LAST];
    bool keyReleased[GLFW_KEY_LAST];
    Window::Window(int width, int height, const std::string& title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        wnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(wnd);

        if (wnd == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initilize GLAD" << std::endl;
        }

        glfwSetFramebufferSizeCallback(wnd, framebuffer_size_callback);
        glfwSetKeyCallback(wnd, key_callback);
    }

    Window::~Window()
    {
        glfwDestroyWindow(wnd);
        glfwTerminate();
    }

    bool Window::shouldClose() const noexcept
    {
        return glfwWindowShouldClose(wnd) != 0;
    }

    void Window::pollEvents() const noexcept
    {
        glfwPollEvents();
    }

    void Window::swapBuffers() const noexcept
    {
        glfwSwapBuffers(wnd);
    }

    void Window::processInput() const noexcept
    {
        if (glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(wnd, true);
        memset(keyReleased, 0, GLFW_KEY_LAST);
    }

    std::pair<int, int> Window::getWindowSize() const noexcept
    {
        std::pair<int, int> sz{};
        glfwGetWindowSize(wnd, &sz.first, &sz.second);
        return sz;
    }

    bool* Window::getKeyPress() const noexcept
    {
        return keyPressed;
    }

    bool * Window::getKeyRelease() const noexcept
    {
        return keyReleased;
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if (action == GLFW_PRESS)
            keyPressed[key] = true;
        if (action == GLFW_RELEASE)
        {
            keyPressed[key] = false;
            keyReleased[key] = true;
        }
    }
}