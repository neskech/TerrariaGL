#include "input/input.h"
#include <string.h>
#include <cassert>  

KeyListener* KeyListener::instance = nullptr;

KeyListener::KeyListener(){
    if (instance != nullptr)
        assert(false);
    instance = this;
}

void KeyListener::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS)
       instance->keys[key] = ( ( (1 << 6) - 1) & mods ) | (1 << 7);   
    else if (action == GLFW_RELEASE)
       instance->keys[key] = 0;
    else if (action == GLFW_REPEAT)
       instance->keys[key] = ( ( (1 << 6) - 1) & mods ) | (0b11 << 6); 
}

bool KeyListener::isKeyPressed(uint16_t keyCode){
    assert(keyCode >= 0 && keyCode < 350);
    return (instance->keys[keyCode] >> 7) & 1;
}

bool KeyListener::isKeyRepeated(uint16_t keyCode){
    assert(keyCode >= 0 && keyCode < 350);
    return (instance->keys[keyCode] >> 6) & 1;
}

bool KeyListener::isKeyModdedBy(uint16_t keyCode, int mod){
    assert(keyCode >= 0 && keyCode < 350);
    return  ( ( ( (1 << 6) - 1) & instance->keys[keyCode] ) & mod );
}

/* 
//////////////////////////////////////////////////////////////////////
////////////////////////--MOUSE LISTENER--////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
*/

//constructor implicitly called on instance at start of program
MouseListener* MouseListener::instance = nullptr;

MouseListener::MouseListener(Window* windowRef){
    if (instance != nullptr)
        assert(false);
    instance = this;
    window = windowRef;
}

MouseListener::~MouseListener(){
    if (cursor != nullptr)
          glfwDestroyCursor(cursor);
    std::cout << "Destructed the mouse listener!\n";
}

void MouseListener::cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
    instance->Mousex = (float)xpos;
    instance->Mousey = (float)ypos;
}
void MouseListener::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //Only 7 bits required
     if (action == GLFW_PRESS)
        instance->buttons[button] = ( ( (1 << 6) - 1) & mods ) | (1 << 6);
     else
         instance->buttons[button] = 0;
}
void MouseListener::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    instance->Mousex = (float)xoffset;
    instance->Mousey = (float)yoffset;
}

void MouseListener::setCursorMode(int cursorMode){
    if (instance->window == nullptr)
        return
    glfwSetInputMode(instance->window->getGLFWwindow(), GLFW_CURSOR, cursorMode);
}

void MouseListener::setInputMode(int type, int mode){
     if (instance->window == nullptr)
        return
     glfwSetInputMode(instance->window->getGLFWwindow(), type, mode);
}

void MouseListener::setDefaultInputMode(){
    if (instance->window == nullptr)
        return
    glfwSetInputMode(instance->window->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool MouseListener::isMouseButtonPressed(int button){
    assert(button >= 0 && button < 3);
    return (instance->buttons[button] >> 6) & 1;
}

bool MouseListener::isMouseButtonModdedBy(int button, int mods){
    assert(button >= 0 && button < 3);
    return ( ( (1 << 6) - 1) & instance->buttons[button] ) & mods;
}

bool MouseListener::setCursorImage(const char* path){
    if (instance->window == nullptr)
        return false;

    std::cout << "im making it!\n";
    Image* raw = loadImageAsRaw(path);
    GLFWimage img;
    img.width = raw->width;
    img.height = raw->height;
    img.pixels = raw->pixels;
    raw->pixels = nullptr;
    delete raw;
    
    if (instance->cursor != nullptr)
        glfwDestroyCursor(instance->cursor);

    instance->cursor = glfwCreateCursor(&img, 0, 0);
    if (instance->cursor == nullptr){
        std::cerr << "ERROR: Mouse cursor creation failed!\n";
        return false;
    }

    glfwSetCursor(instance->window->getGLFWwindow(), instance->cursor);
    return true;
}

void MouseListener::resetCursor(){
    if (instance->window == nullptr)
        return

    glfwDestroyCursor(instance->cursor);
    glfwSetCursor(instance->window->getGLFWwindow(), NULL);
}


