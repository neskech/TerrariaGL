#pragma once
#include <GLFW/glfw3.h>

class Window{
    public:
        Window(unsigned int width_, unsigned int height_);
        ~Window();

        void run();
        void terminate();

        inline unsigned int getWidth(){ return width; }
        inline unsigned int getHeight(){ return height; }

    private:
        GLFWwindow* window;
        unsigned int width, height;
};