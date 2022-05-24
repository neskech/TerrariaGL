#include "input/input.h"
#include <cassert>  

KeyListener KeyListener::instance;

void KeyListener::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS)
        KeyListener::instance.keys[key] = ( ( (1 << 6) - 1) & mods ) | (1 << 7);   
    else if (action == GLFW_RELEASE)
        KeyListener::instance.keys[key] = 0;
    else if (action == GLFW_REPEAT)
        KeyListener::instance.keys[key] = ( ( (1 << 6) - 1) & mods ) | (0b11 << 6); 
}

bool KeyListener::isKeyPressed(uint16_t keyCode){
    assert(keyCode >= 0 && keyCode < 350);
    return (KeyListener::instance.keys[keyCode] >> 7) & 1;
}

bool KeyListener::isKeyRepeated(uint16_t keyCode){
    assert(keyCode >= 0 && keyCode < 350);
    return (KeyListener::instance.keys[keyCode] >> 6) & 1;
}

bool KeyListener::isKeyModdedBy(uint16_t keyCode, int mod){
    assert(keyCode >= 0 && keyCode < 350);
    return ( ( (1 << 6) - 1) & KeyListener::instance.keys[keyCode] ) & mod;
}

/* 
//////////////////////////////////////////////////////////////////////
////////////////////////--MOUSE LISTENER--////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
*/

//constructor implicitly called on instance at start of program
MouseListener MouseListener::instance;

MouseListener::~MouseListener(){
    if (cursor != nullptr)
          glfwDestroyCursor(cursor);
    std::cout << "Destructed the mouse listener!";
}

void MouseListener::cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
    MouseListener::instance.Mousex = (float)xpos;
    MouseListener::instance.Mousey = (float)ypos;
}
void MouseListener::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //Only 7 bits required
     if (action == GLFW_PRESS)
        MouseListener::instance.buttons[button] = ( ( (1 << 6) - 1) & mods ) | (1 << 6);
     else
         MouseListener::instance.buttons[button] = 0;
}
void MouseListener::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    MouseListener::instance.Mousex = (float)xoffset;
    MouseListener::instance.Mousey = (float)yoffset;
}

void MouseListener::setInputMode(GLFWwindow* window, int cursor, int cursorMode){

}
void MouseListener::setDefaultInputMode(GLFWwindow* window){

}
bool MouseListener::isMouseInWindow(GLFWwindow* window){

}

void MouseListener::setCursorImage(Ref<Image>){

}


