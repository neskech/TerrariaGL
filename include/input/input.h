#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

#include "window.h"
#include "image.h"
#include "pch.h"

class KeyListener{
    public:
        KeyListener();
        ~KeyListener(){}

        static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
        static bool isKeyPressed(uint16_t keyCode);
        static bool isKeyRepeated(uint16_t keyCode);
        static bool isKeyModdedBy(uint16_t keyCode, int mods);
      
    private:
        static KeyListener* instance;
        int8_t keys[350];
};


 class MouseListener{
     public:
        MouseListener(Window* windowRef);
       ~MouseListener();

        static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        
        static void setCursorMode(int cursorMode);
        static void setInputMode(int type, int mode);
        static void setDefaultInputMode();

        static bool isMouseButtonPressed(int button);
        static bool isMouseButtonModdedBy(int button, int mod);

        static bool setCursorImage(const char* path);
        static void resetCursor();

        static inline int GetMousex(){ return instance->Mousex; }
        static inline int GetMousey(){ return instance->Mousey; }

     private:
        static MouseListener* instance; 

        int8_t buttons[3];
        float Mousex, Mousey;
        float ScrollX, ScrollY;
        GLFWcursor* cursor = nullptr;
        Window* window = nullptr;

 };



