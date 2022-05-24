#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

class Window{
    public:
         Window(uint32_t width_, uint32_t height_);
        ~Window();

        Window(const Window& other) = delete;
        Window(Window&& other) = delete;

        bool init();
        void setupCallbacks();
        void update();

        inline GLFWwindow* getGLFWwindow() { return glfw_window; } 
        inline bool windowClosing() { return glfwWindowShouldClose(glfw_window); }
        inline unsigned int getWidth(){ return width; }
        inline unsigned int getHeight(){ return height; }

    private:
        static Window* instance;
        GLFWwindow* glfw_window;
        uint32_t width, height;

        static void onWindowResize(int newWidth, int newHeight);
};