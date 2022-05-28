#include "core/application.h"

#include "renderer/buffer.h"
#include "renderer/shader.h"
#include "renderer/VAO.h"

Application* Application::instance = nullptr;

Application::Application(uint32_t windowWidth, uint32_t windowHeight){
    if (instance != nullptr)
        assert(false);

    instance = this;
    window = new Window(windowWidth, windowHeight);
    keyListener = new KeyListener();
    mouseListener = new MouseListener(window);
    assetPool = new AssetPool();
}

Application::~Application(){
    delete assetPool;
    delete keyListener;
    delete mouseListener;
    delete window;
}

bool Application::init(){
    if (!window->init()){
        std::cerr << "ERROR: Application --> Window initialization failed!\n";
        return false;
    }
    std::cout << "Done with init!\n";
}

void Application::run(){
    double delta;
    double end = 0.0;
    double now;


    Ref<Shader> shader = AssetPool::getShader("/Users/shauntemellor/Documents/comsci/Projects/Terraria/assets/shaders/test.glsl");
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // left  
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,// right 
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f // top   
    }; 

    Buffer VBO(BufferType::VBO);
    VertexArrayObject VAO(6 * sizeof(float));

    VAO.bind();

    VBO.bind();
    VBO.addData<float>(vertices, 18, 0, GL_STATIC_DRAW);

    //make data type and data size (assuming they stay constant) member variables of the VAO
    VAO.addAtribute(3, GL_FLOAT, sizeof(float));
    VAO.addAtribute(3, GL_FLOAT, sizeof(float));

    //VBO.unBind();

    //VAO.unBind();
    while (!window->windowClosing()){
        now = glfwGetTime();
        end = now;

        window->update();

        VAO.bind();
        shader->use();
        glDrawArrays(GL_TRIANGLES, 0, 3);


        window->finishFrame();
        window->pollEvents();
    }
}