#include "renderer/debugRenderer.h"
#include "util/assetPool.h"
#include "scene/camera.h"

#define VERTEX_SIZE 5

DebugRenderer* DebugRenderer::instance = nullptr;

DebugRenderer::DebugRenderer(): VBO(BufferType::VBO), VAO(), numLines(0){
    if (instance != nullptr)
        assert(false);
    instance = this;
}

DebugRenderer::~DebugRenderer(){

}

void DebugRenderer::init(){
    VAO.bind();

    VBO.bind();
    VBO.allocateData<float>(MAX_DEBUG_OBJECTS * VERTEX_SIZE, GL_DYNAMIC_DRAW);

    //Position
    VAO.addAtribute(2, GL_FLOAT, sizeof(float), VERTEX_SIZE * sizeof(float));
    //Color -- RGB
    VAO.addAtribute(3, GL_FLOAT, sizeof(float), VERTEX_SIZE * sizeof(float));

    VBO.unBind();
    VAO.unBind();

    shader = AssetPool::getShader("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/shaders/debugShader.glsl");
}

void DebugRenderer::render(){
    shader->activate();

    VBO.bind();

    shader->setmat4("ortho", Camera::getProjectionMatrix());
    shader->setmat4("view", Camera::getViewMatrix());
    
    VAO.bind();
    glDrawArrays(GL_LINES, 0, numLines * 2);
    VAO.unBind();

    shader->deActivate();
    VBO.unBind();
    numLines = 0;
}

void DebugRenderer::drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color){
    instance->VBO.bind();
    float* vertexData = instance->VBO.mapBuffer<float>();
    int offset = instance->numLines * 2 * VERTEX_SIZE;

    vertexData[offset + 0] = (start.x);
    vertexData[offset + 1] = (start.y);
    vertexData[offset + 2] = (color.x);
    vertexData[offset + 3] = (color.y);
    vertexData[offset + 4] = (color.z);

    vertexData[offset + 5] = (end.x);
    vertexData[offset + 6] = (end.y);
    vertexData[offset + 7] = (color.x);
    vertexData[offset + 8] = (color.y);
    vertexData[offset + 9] = (color.z);

    instance->VBO.unMapBuffer();
    instance->VBO.unBind();
    instance->numLines++;
}

void DebugRenderer::drawRect(const glm::vec2& bottomLeft, const glm::vec2& topRight, const glm::vec3& color){
    glm::vec2 points[4] = {
        bottomLeft + glm::vec2(0.0f, topRight.y - bottomLeft.y), //topLeft
        topRight,                                                //topRight
        bottomLeft + glm::vec2(topRight.x - bottomLeft.x, 0.0f), //bottomRight
        bottomLeft                                               //bottomLeft
    };

    for (int i = 0; i < 4; i++)
        drawLine(points[i], points[(i + 1) % 4], color);

}