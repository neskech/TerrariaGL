#pragma once
#include "pch.h"


class Window{
    public:
         Window(uint32_t width_, uint32_t height_);
        ~Window();

        Window(const Window& other) = delete;
        Window(Window&& other) = delete;

        bool init();
        bool initOPENGL();
        void setupCallbacks();
        void update();
        void pollEvents();
        void finishFrame();

        inline GLFWwindow* getGLFWwindow() { return glfw_window; } 
        inline bool windowClosing() { return glfwWindowShouldClose(glfw_window); }
        inline static unsigned int getWidth(){ return instance->width; }
        inline static unsigned int getHeight(){ return instance->height; }

    private:
        static Window* instance;
        GLFWwindow* glfw_window;
        uint32_t width, height;

        static void error_callback(int error, const char* description);
        static void resize_callback(GLFWwindow *window, int32_t width, int32_t height);
};