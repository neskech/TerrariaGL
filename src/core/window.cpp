#include "core/window.h"
#include "core/input.h"
#include "core/application.h"

Window* Window::instance = nullptr;

void Window::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void Window::resize_callback(GLFWwindow *window, int32_t width_, int32_t height_){
     instance->width = width_;
     instance->height = height_;
}


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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfw_window = glfwCreateWindow(width, height, "Terraria", NULL, NULL);
    if (glfw_window == nullptr)
    {
        std::cerr << "ERROR: Window failed to initialize\n";
        glfwTerminate();
        return false;
    }

    setupCallbacks();

    glfwMakeContextCurrent(glfw_window);

    if (!initOPENGL()){
        std::cout << "failed to initialze OPENGL" << std::endl;
        return false;
    }

    return true;
}

bool Window::initOPENGL(){
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "failed to initialze glad" << std::endl;
        return false;
    }

    glfwSwapInterval(1);
    
    glEnable(GL_BLEND);  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
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
    if (KeyListener::isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(glfw_window, true);

   // glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.9f, 0.95f, 0.9f, 1.0f);
}

void Window::pollEvents(){
    glfwPollEvents();
}

void Window::finishFrame(){
     glfwSwapBuffers(glfw_window);
}
