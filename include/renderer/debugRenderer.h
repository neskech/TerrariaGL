#pragma once
#include "renderer/buffer.h"
#include "renderer/VAO.h"
#include "renderer/shader.h"

#define MAX_DEBUG_OBJECTS 1000

class DebugRenderer{
    public:
        DebugRenderer();
        DebugRenderer(DebugRenderer& other) = delete;
        DebugRenderer(const DebugRenderer& other) = delete;
        DebugRenderer(DebugRenderer&& other) = delete;
        ~DebugRenderer();

        void init();
        void render();
        static void drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
        static void drawRect(const glm::vec2& bottomLeft, const glm::vec2& topRight, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));

    private:
        static DebugRenderer* instance;

        Buffer VBO;
        VertexArrayObject VAO;
        Ref<Shader> shader;
        int numLines; 

};