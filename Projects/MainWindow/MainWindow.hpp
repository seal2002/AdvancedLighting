#ifndef MAINWINDOW_LIB_HPP
#define MAINWINDOW_LIB_HPP 1

#include <string>
#include <memory>
#include <iostream>

#include <glad/glad.h>

#include "common/Camera.h"

// Forward declare GLFWwindow to avoid including glfw3.h
struct GLFWwindow;

namespace OpenGLWindow
{
    class Window
    {
    public:
        Window(int width, int  height, const std::string& title);
        ~Window();

        bool shouldClose() const noexcept;

        void pollEvents() const noexcept;

        void swapBuffers() const noexcept;

        void processInput() const noexcept;

        void movement(Camera &camera) const noexcept;

        std::pair<int, int> getWindowSize() const noexcept;

        bool * getKeyPress() const noexcept;

        bool * getKeyRelease() const noexcept;

    private:
        GLFWwindow* wnd;
    };
}

#endif