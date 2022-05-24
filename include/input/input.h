#pragma once
#include <GLFW/glfw3.h>
#include "image.h"
#include <iostream>
#include "pch.h"

class KeyListener{
    public:
        static void init();

        static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
        static bool isKeyPressed(uint16_t keyCode);
        static bool isKeyRepeated(uint16_t keyCode);
        static bool isKeyModdedBy(uint16_t keyCode, int mods);
      
    private:
        KeyListener() {std::cout<<"Made the KeyListener!";};
        static KeyListener instance;
        int8_t keys[350];
};


 class MouseListener{
     public:
        static void init();

        static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        
        static void setInputMode(int cursor, int cursorMode);
        static void setDefaultInputMode();

        static bool isMouseButtonModdedBy()
        static bool isMouseInWindow();

        static void setCursorImage(Ref<Image>);

        static inline int GetMousex(){ return MouseListener::instance.Mousex; }
        static inline int GetMousey(){ return MouseListener::instance.Mousey; }

        ~MouseListener();
     private:
        MouseListener() {std::cout << "Constructed the Mouse Listener!";};
        static MouseListener instance; 

        int8_t buttons[3];
        float Mousex, Mousey;
        float ScrollX, ScrollY;
        GLFWcursor* cursor;

        WeakRef<GLFWwindow> window;

 };



