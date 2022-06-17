#pragma once
#include "renderer/buffer.h"
#include "renderer/VAO.h"
#include "renderer/shader.h"
#include "renderer/texture.h"

class FontRenderer{
    public:
        FontRenderer();
        FontRenderer(FontRenderer& other) = delete;
        FontRenderer(const FontRenderer& other) = delete;
        FontRenderer(FontRenderer&& other) = delete;
        ~FontRenderer(){}

        void init();
        void render();
        static void drawText(const std::string& text, const glm::vec2& pos, float charSize, float gap, const glm::vec3& color);

    private:
        void setIndexData();
        static FontRenderer* instance;
        
        Buffer VBO;
        Buffer EBO;
        VertexArrayObject VAO;
        Ref<Shader> shader;
        Ref<Texture> fontAtlas;
        int numLetters;

};