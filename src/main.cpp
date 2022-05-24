#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "input/input.h"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(){
    std::cout<<"start program!";
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()){
        std::cerr << "ERROR: GLFW failed to initialize\n";
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //Modern opengl functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Terraria", NULL, NULL);
    if (!window)
    {
        std::cerr << "ERROR: Window failed to initialize\n";
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, KeyListener::keyCallBack);
    glfwSetCursorPosCallback(window, MouseListener::cursor_position_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    int width = 300, height = 300;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    std::cout<<"Before loop!";
    while (!glfwWindowShouldClose(window))
    {
        if (KeyListener::isKeyPressed(GLFW_KEY_A))
            std::cout << MouseListener::GetMousex() << std::endl;
        // Keep running
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    return 0;
}

