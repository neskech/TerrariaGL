#include "window.h"
#include "input/input.h"


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

Window* Window::instance = nullptr;


Window::Window(uint32_t width_, uint32_t height_){
         Window::instance = this;
         width = width_;
         height = height_;
}

bool Window::init(){

    if (!glfwInit()){
        std::cerr << "ERROR: GLFW failed to initialize\n";
        glfwTerminate();
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::cout << width << " " << height << std::endl;
    glfw_window = glfwCreateWindow(width, height, "Terraria", NULL, NULL);
    if (glfw_window == nullptr)
    {
        std::cerr << "ERROR: Window failed to initialize\n";
        glfwTerminate();
        return false;
    }

    setupCallbacks();

    glfwMakeContextCurrent(glfw_window);
    gladLoadGL();
    glfwSwapInterval(1);
    glViewport(0, 0, width, height);

    return true;
}

void Window::setupCallbacks(){
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(glfw_window, KeyListener::keyCallBack);
    glfwSetCursorPosCallback(glfw_window, MouseListener::cursor_position_callback);
    glfwSetMouseButtonCallback(glfw_window, MouseListener::mouse_button_callback);
    glfwSetScrollCallback(glfw_window, MouseListener::scroll_callback);
}

Window::~Window(){
    std::cout << "Destroyed the window!\n";
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}

void Window::update(){
     glfwSwapBuffers(glfw_window);
     glfwPollEvents();
}
