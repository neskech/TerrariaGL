#include "renderer/renderBatch.h"
#include "scene/camera.h"

static int TEX_SLOTS[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

RenderBatch::RenderBatch(){
    spriteRends.reserve(MAX_BATCH_SIZE);
    shader = AssetPool::getShader("../../assets/shaders/test.glsl");
}

RenderBatch::~RenderBatch(){

}

void RenderBatch::init(){
    VAO.bind();
    VBO.bind();
    EBO.bind();

    EBO.allocateData<float>(MAX_BATCH_SIZE * 3 * 2);
    VBO.allocateData<float>(MAX_BATCH_SIZE * VERTEX_SIZE);

    //pos
    VAO.addAtribute(2, GL_FLOAT, sizeof(float));
    //uv
    VAO.addAtribute(2, GL_FLOAT, sizeof(float));
    //TexID
    VAO.addAtribute(1, GL_FLOAT, sizeof(float));

    VBO.unBind();
    VAO.unBind();
}

void RenderBatch::render(){
    VAO.bind();
    EBO.bind();
    VBO.bind();

    shader->use();
    shader->uploadTextures("u_textures", TEX_SLOTS);
    shader->setmat4("ortho", Camera::getProjectionMatrix());
    shader->setmat4("view", Camera::getViewMatrix());

    for (int i = 0; i < 8; i++){
          glActiveTexture(GL_TEXTURE0 + i);
          textures[i]->bind();
    }

    glDrawElements(GL_TRIANGLES, numSprites * 6, GL_STATIC_DRAW, 0);

    shader->deActivate();

    for (int i = 0; i < 8; i++){
          glActiveTexture(GL_TEXTURE0 + i);
          textures[i]->bind();
    }

    VAO.unBind();
}

void RenderBatch::addSpriteRenderer(Component::SpriteRenderer* spr){ 
    int16_t texID = -1;
    for (int i = 0; i < MAX_TEXTURES; i++){
        if (spr->sheet.tex == textures[i]){
            texID = i;
            break;
        }
    }
    
    if (texID == -1){
        textures[numTextures] = spr->sheet.tex;
        texID = numTextures++;
    }

    configureSpriteVertexData(spr, texID);

    spriteRends[spr] = numSprites;
    numSprites++;

}

void RenderBatch::configureSpriteVertexData(Component::SpriteRenderer* spr, int16_t texID){
    VBO.bind();

    float* buffer_ptr = VBO.mapBuffer<float>();

    //const glm::vec2 pos = spr
    glm::vec2 uvs(spr->sheet.cellWidth * spr->sprite.col, spr->sheet.cellHeight * spr->sprite.row);
    uvs.x /= spr->sheet.tex->getWidth();
    uvs.y /= spr->sheet.tex->getHeight();

    const glm::vec2& pos = spr->parent.getComponent<Component::Transform>().position;
    
    //four vertices per spriteRenderer 
    int offset = numSprites * VERTEX_SIZE * 4;
    for (int i = 0; i < 4; i++){
        buffer_ptr[offset + i * VERTEX_SIZE + 0] = pos.x;
        buffer_ptr[offset + i * VERTEX_SIZE + 1] = pos.y;
        buffer_ptr[offset + i * VERTEX_SIZE + 2] = uvs.x;
        buffer_ptr[offset + i * VERTEX_SIZE + 3] = uvs.y;
        buffer_ptr[offset + i * VERTEX_SIZE + 4] = texID;
    }

    VBO.unMapBuffer();
}

void RenderBatch::removeSpriteRenderer(Component::SpriteRenderer* spr){
     VBO.bind();
     uint16_t index = spriteRends[spr];
     uint16_t start = index * VERTEX_SIZE * 4;

     float* buffer_ptr = VBO.mapBuffer<float>();
     for (uint16_t i = start; i < numSprites - 1; i++){
         buffer_ptr[i] = buffer_ptr[i + VERTEX_SIZE];
     }
     VBO.unMapBuffer();

     spriteRends.erase(spr);
     numSprites--;

}

void RenderBatch::generateIndices(){
    EBO.bind();
    int* buffer_ptr = EBO.mapBuffer<int>();
    for (int i = 0; i < MAX_BATCH_SIZE * 3 * 2; i++){
        buffer_ptr[ 0 + i * 6 ] = 3 + 4 * i;
        buffer_ptr[ 1 + i * 6 ] = 2 + 4 * i;
        buffer_ptr[ 2 + i * 6 ] = 0 + 4 * i;
        buffer_ptr[ 3 + i * 6 ] = 0 + 4 * i;
        buffer_ptr[ 4 + i * 6 ] = 2 + 4 * i;
        buffer_ptr[ 5 + i * 6 ] = 1 + 4 * i;
    }
    EBO.unMapBuffer();
}

void RenderBatch::updateDirtyFlags(){
    for (auto it : spriteRends){
        if (it.first->dirty || it.first->parent.getComponent<Component::Transform>().dirty){
            int16_t texID;
            for (int i = 0; i < MAX_TEXTURES; i++){
                if (it.first->sheet.tex == textures[i]){
                    texID = i;
                    break;
                }
            }
            configureSpriteVertexData(it.first, texID);
        }
    }

}

bool RenderBatch::containsTexture(const Ref<Texture>& tex){
    for (auto& text : textures){
        if (tex == text)
            return true;
    }
    return false;
}


