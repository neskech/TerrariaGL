#pragma once
#include "pch.h"

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
        void pollEvents();
        void finishFrame();

        inline GLFWwindow* getGLFWwindow() { return glfw_window; } 
        inline bool windowClosing() { return glfwWindowShouldClose(glfw_window); }
        inline unsigned int getWidth(){ return width; }
        inline unsigned int getHeight(){ return height; }

    private:
        static Window* instance;
        GLFWwindow* glfw_window;
        uint32_t width, height;

        static void error_callback(int error, const char* description);
        static void resize_callback(GLFWwindow *window, int32_t width, int32_t height);
};