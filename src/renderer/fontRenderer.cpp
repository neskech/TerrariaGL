 #include "renderer/fontRenderer.h"

#include "renderer/fontRenderer.h"
#include "util/assetPool.h"
#include "scene/camera.h"
 
#define VERTEX_SIZE 7
#define TEXT_OFFSET 33
#define MAX_CHARACTERS 1000

#define ATLAS_DIMENSIONS_X 300.0f
#define ATLAS_DIMENSIONS_Y 160.0f
#define NUM_CELLS_X 15
#define NUM_CELLS_Y 8
#define GLYPH_DIMENSIONS_X 20.0f
#define GLYPH_DIMENSIONS_Y 20.0f


 FontRenderer* FontRenderer::instance = nullptr;

 FontRenderer::FontRenderer(): VBO(BufferType::VBO), EBO(BufferType::EBO),
 VAO(), numLetters(0)
 {
     if (instance != nullptr)
        assert(false);
     instance = this;
 }

void FontRenderer::init(){
    VAO.bind();

    VBO.bind();
    VBO.allocateData<float>(MAX_CHARACTERS * 4 * VERTEX_SIZE, GL_DYNAMIC_DRAW);

    EBO.bind();
    EBO.allocateData<unsigned int>(MAX_CHARACTERS * 6, GL_STATIC_DRAW);
    setIndexData();

    //Position
    VAO.addAtribute(2, GL_FLOAT, sizeof(float), VERTEX_SIZE * sizeof(float));
    //UVs
    VAO.addAtribute(2, GL_FLOAT, sizeof(float), VERTEX_SIZE * sizeof(float));
    //Color -- RGB
    VAO.addAtribute(3, GL_FLOAT, sizeof(float), VERTEX_SIZE * sizeof(float));

    VBO.unBind();
    VAO.unBind();
    EBO.unBind();

    TexParams params{
        .behaviorX = GL_CLAMP_TO_BORDER,
        .behaviorY = GL_CLAMP_TO_BORDER,
        .magFilter = GL_NEAREST,
        .minFilter = GL_NEAREST,
        .mipMapLevels = 1
    };
    fontAtlas = AssetPool::getTexture("../assets/img/font.png", params);
    shader = AssetPool::getShader("../assets/shaders/textShader.glsl");
}

void FontRenderer::drawText(const std::string& text, const glm::vec2& pos, float charSize, float gap, const glm::vec3& color){

    instance->VBO.bind();
    float* buffer_ptr = instance->VBO.mapBuffer<float>();

    for (int c = 0; c < text.length(); c++){

        int offset = instance->numLetters * 4 * VERTEX_SIZE + c * 4 * VERTEX_SIZE;
        int offsetX = c * (gap + charSize);
        int texID = ((int) text[c]) + 1;

        for (int i = 0; i < 4; i++){
            int row = i < 2;
            int col = i == 1 || i == 3;

            buffer_ptr[offset + i * VERTEX_SIZE + 0] = offsetX + pos.x + col * charSize;
            buffer_ptr[offset + i * VERTEX_SIZE + 1] = pos.y - row * charSize;

            buffer_ptr[offset + i * VERTEX_SIZE + 2] = ( ( (texID - TEXT_OFFSET) % NUM_CELLS_X ) * GLYPH_DIMENSIONS_X + col * GLYPH_DIMENSIONS_X ) / ATLAS_DIMENSIONS_X;
            buffer_ptr[offset + i * VERTEX_SIZE + 3] = ( ( (texID - TEXT_OFFSET) / NUM_CELLS_X ) * GLYPH_DIMENSIONS_Y + row * GLYPH_DIMENSIONS_Y ) / ATLAS_DIMENSIONS_Y;

            buffer_ptr[offset + i * VERTEX_SIZE + 4] = color.r;
            buffer_ptr[offset + i * VERTEX_SIZE + 5] = color.g;
            buffer_ptr[offset + i * VERTEX_SIZE + 6] = color.b;
        }

    }

    instance->VBO.unMapBuffer();
    instance->VBO.unBind();
    instance->numLetters += text.length();
}



void FontRenderer::render(){
    shader->activate();

    VBO.bind();

    shader->setmat4("ortho", Camera::getProjectionMatrix());
    shader->setmat4("view", Camera::getViewMatrix());

    fontAtlas->bind();
    
    VAO.bind();
    EBO.bind();
    glDrawElements(GL_TRIANGLES, numLetters * 6, GL_UNSIGNED_INT, 0);
    VAO.unBind();

    fontAtlas->unBind();

    shader->deActivate();
    VBO.unBind();
    EBO.unBind();
    numLetters = 0;
}

void FontRenderer::setIndexData(){
    EBO.bind();
    unsigned int* buffer_ptr = EBO.mapBuffer<unsigned int>();

    for (int i = 0; i < MAX_CHARACTERS; i++){
        buffer_ptr[0 + i * 6] = 0 + i * 4;
        buffer_ptr[1 + i * 6] = 1 + i * 4;
        buffer_ptr[2 + i * 6] = 3 + i * 4;
        buffer_ptr[3 + i * 6] = 0 + i * 4;
        buffer_ptr[4 + i * 6] = 3 + i * 4;
        buffer_ptr[5 + i * 6] = 2 + i * 4;
    }

    EBO.unMapBuffer();
    EBO.unBind();
}
