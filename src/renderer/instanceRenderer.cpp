#include "renderer/instanceRenderer.h"
#include "renderer/renderer.h"
#include "scene/camera.h"

static int TEX_SLOTS[9] = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr int TRANSFORM_VBO_VERT_SIZE = 16;
static constexpr int UV_VBO_VERT_SIZE = 2;
static constexpr int TEXID_VBO_VERT_SIZE = 1;

InstanceRenderer::InstanceRenderer(){
    spriteRends.reserve(MAX_INSTANCES);
    numTextures = 0;
    numSprites = 0;
}

InstanceRenderer::~InstanceRenderer(){
    delete instance_transform_VBO;
    delete instance_uv_VBO;
    delete instance_texID_VBO;
    delete templateVBO;
    delete VAO;
}

void InstanceRenderer::init(){
    shader = AssetPool::getShader("../assets/shaders/test.glsl");

    instance_transform_VBO = new Buffer(BufferType::VBO);
    instance_uv_VBO = new Buffer(BufferType::VBO);
    instance_texID_VBO = new Buffer(BufferType::VBO);
    templateVBO = new Buffer(BufferType::VBO);
    VAO = new VertexArrayObject(sizeof(float) * 2);

    //////////////////////////////////////////////////
    VAO->bind();
    

    float quad[] = {
        -0.5f, 0.5f, //top left
        -0.5f, -0.5f, //bottom left
         0.5f, -0.5f, //bottom right

         0.5f, 0.5f, //top right
        -0.5f, 0.5f, //top left
         0.5f, -0.5f, //bottom right

    };

    templateVBO->bind();
    templateVBO->addData(quad, 12, 0, GL_STATIC_DRAW);

    //pos -- template QUAD
    VAO->addAtribute(2, GL_FLOAT, sizeof(float));

    instance_uv_VBO->bind();
    instance_uv_VBO->allocateData<float>(MAX_INSTANCES * UV_VBO_VERT_SIZE);

    //uv -- passed every vertex
    VAO->resetByteCount();
    VAO->addAtribute(2, GL_FLOAT, sizeof(float), 0);

    instance_texID_VBO->bind();
    instance_texID_VBO->allocateData<float>(MAX_INSTANCES * TEXID_VBO_VERT_SIZE);

    //TexID -- passed every instance
    VAO->resetByteCount();
    VAO->addAtribute(1, GL_FLOAT, sizeof(float), 1);

    instance_transform_VBO->bind();
    instance_transform_VBO->allocateData<float>(MAX_INSTANCES * TRANSFORM_VBO_VERT_SIZE);

    //Transform -- passed every instance
    VAO->resetByteCount();
    VAO->addMatrixAttribute(4, 4, true);

    templateVBO->unBind();
    instance_transform_VBO->unBind();
    instance_uv_VBO->unBind();
    instance_texID_VBO->unBind();
    VAO->unBind();
}

void InstanceRenderer::render(){
    VAO->bind();

    shader->use();
   ////// shader->uploadTextures("uTextures", TEX_SLOTS);
    shader->setmat4("ortho", Camera::getProjectionMatrix());
    shader->setmat4("view", Camera::getViewMatrix());


    for (int i = 0; i < numTextures; i++){
          glActiveTexture(GL_TEXTURE0 + i);
          textures[i]->bind();
          shader->uploadTexture("uTextures", i);
    }

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numSprites);


    shader->deActivate();
    for (int i = 0; i < numTextures; i++){
          textures[i]->unBind();
    }

    VAO->unBind();
 
}

void InstanceRenderer::addSpriteRenderer(Component::SpriteRenderer* spr){ 
    int16_t texID = -1;
    for (int i = 0; i < numTextures; i++){

        if (spr->sheet.tex.get() == textures[i]){
            texID = i;
            break;
        }
    }
    
    if (texID == -1){
        textures[numTextures] = spr->sheet.tex.get();
        texID = numTextures++;
    }
    std::cout << "About to add! " << numSprites << " " << texID << std::endl;
    configureSpriteVertexData(spr, texID);

    spriteRends[spr] = numSprites;
    numSprites++;

}

void InstanceRenderer::configureSpriteVertexData(Component::SpriteRenderer* spr, int16_t texID){
    instance_uv_VBO->bind();

    //uvs
    float* buffer_ptr = instance_uv_VBO->mapBuffer<float>();

    glm::vec2 uvs(spr->sheet.cellWidth * spr->sprite.col, spr->sheet.cellHeight * spr->sprite.row);

    int row, col;
    int offset = numSprites * UV_VBO_VERT_SIZE * 4;
    for (int i = 0; i < 4; i++){
        row = i / 2;
        col = i % 2;

        buffer_ptr[offset + i * UV_VBO_VERT_SIZE + 0] = (uvs.x + spr->sheet.cellWidth * col) / spr->sheet.tex->getWidth();
        buffer_ptr[offset + i * UV_VBO_VERT_SIZE + 1] = (uvs.y + spr->sheet.cellHeight * row) / spr->sheet.tex->getHeight();

    }

    instance_uv_VBO->unMapBuffer();

    //////   //////   //////   //////   //////   //////   //////   //////   //////   //////   //////

    instance_texID_VBO->bind();

    //texID
    buffer_ptr = instance_texID_VBO->mapBuffer<float>();
    offset = numSprites * UV_VBO_VERT_SIZE;
    buffer_ptr[offset] = texID;
    instance_texID_VBO->unMapBuffer();

    ///////////////////////////////////////////////////////////////////////////////////////////////

    instance_transform_VBO->bind();
    //transform
    const glm::mat4& trans = spr->parent.getComponent<Component::Transform>().getTransform();

    buffer_ptr = instance_transform_VBO->mapBuffer<float>();

    offset = numSprites * TRANSFORM_VBO_VERT_SIZE;

    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            int index = r * 4 + c;
            buffer_ptr[offset + index] = trans[r][c];
        }
    }

    instance_transform_VBO->unMapBuffer();
    //TODO unbind all three so they dont get accidently modified by another method
    
}

void InstanceRenderer::removeSpriteRenderer(Component::SpriteRenderer* spr){
    //  uint16_t index = spriteRends[spr];
    //  instance_texture_VBO->bind();

    //  uint16_t start = index * TEXTURE_VBO_VERT_SIZE * 4;
    //  float* buffer_ptr = instance_texture_VBO->mapBuffer<float>();
    //  for (uint16_t i = start; i < numSprites * TEXTURE_VBO_VERT_SIZE * 4 - TEXTURE_VBO_VERT_SIZE; i++){
    //      buffer_ptr[i] = buffer_ptr[i + TEXTURE_VBO_VERT_SIZE];
    //  }
    //  instance_texture_VBO->unMapBuffer();

    //  start = index * TRANSFORM_VBO_VERT_SIZE;
    //  buffer_ptr = instance_transform_VBO->mapBuffer<float>();
    //  for (uint16_t i = start; i < numSprites * TRANSFORM_VBO_VERT_SIZE - TRANSFORM_VBO_VERT_SIZE; i++){
    //      buffer_ptr[i] = buffer_ptr[i + TRANSFORM_VBO_VERT_SIZE];
    //  }
    //  instance_transform_VBO->unMapBuffer();

    //  spriteRends.erase(spr);
    //  numSprites--;

}

void InstanceRenderer::updateDirtyFlags(){
    for (auto it : spriteRends){
        if (it.first->dirty || it.first->parent.getComponent<Component::Transform>().dirty){
            int16_t texID;
            for (int i = 0; i < MAX_TEXTURES; i++){
                if (it.first->sheet.tex.get() == textures[i]){
                    texID = i;
                    break;
                }
            }
            configureSpriteVertexData(it.first, texID);
        }
    }

}

bool InstanceRenderer::containsTexture(const Ref<Texture>& tex){
    for (auto& text : textures){
        if (tex.get() == text)
            return true;
    }
    return false;
}

