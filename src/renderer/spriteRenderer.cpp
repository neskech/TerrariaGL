#include "renderer/spriteRenderer.h"
#include "renderer/renderer.h"
#include "scene/camera.h"

#define TEXCORD_VERT_SIZE 1
#define TEXID_VERT_SIZE 1
#define COLOR_VERT_SIZE 4
#define TRANSFORM_VERT_SIZE 16
#define VERTEX_SIZE 22


SpriteRenderer::SpriteRenderer(Renderer& renderer_, int zIndex_):
    numSprites(0), numSpriteSheets(0), zIndex(zIndex_),
    instanceVBO(BufferType::VBO), templateVBO(BufferType::VBO),
    templateEBO(BufferType::EBO), VAO(), renderer(renderer_)
{
    //index_map.reserve(MAX_INSTANCES);
}

SpriteRenderer::~SpriteRenderer(){

}   

void SpriteRenderer::init(){
    shader = AssetPool::getShader("../assets/shaders/spriteShader.glsl");
    VAO.bind();
    
    float quad[] = {
        -0.5f, -0.5f, //bottom left 0
         0.5f, -0.5f, //bottom right 1
        -0.5f, 0.5f, //top left 2
         0.5f, 0.5f, //top right 3
    };

    unsigned int indices[] = {
            0, 1, 3,
            0, 3, 2,
    };
    
    templateEBO.bind();
    templateEBO.addData(indices, 6, 0, GL_STATIC_DRAW);

    templateVBO.bind();
    templateVBO.addData(quad, 8, 0, GL_STATIC_DRAW);    

    //Pos -- template QUAD
    VAO.addAtribute(2, GL_FLOAT, sizeof(float), sizeof(float) * 2);
    templateVBO.unBind();

    instanceVBO.bind();
    instanceVBO.allocateData<float>((MAX_INSTANCES / MAX_INSTANCE_RENDERERS) * VERTEX_SIZE, GL_DYNAMIC_DRAW);

    float vertexSizeBytes = VERTEX_SIZE * sizeof(float);
    //1D Texture Coordinate -- passed every instance
    VAO.resetByteCount();
    VAO.addAtribute(1, GL_FLOAT, sizeof(float), vertexSizeBytes, 1);

    //TexID -- passed every instance
    VAO.addAtribute(1, GL_FLOAT, sizeof(float), vertexSizeBytes, 1);

    //Color Vec4 -- passed every instance
    VAO.addAtribute(4, GL_FLOAT, sizeof(float), vertexSizeBytes, 1);

    //Transform -- passed every instance
    VAO.addMatrixAttribute(4, 4, vertexSizeBytes, 1);

    templateEBO.unBind();
    instanceVBO.unBind();
    VAO.unBind();
}

void SpriteRenderer::render(){
    VAO.bind();

    shader->activate();
    shader->setmat4("ortho", Camera::getProjectionMatrix());
    shader->setmat4("view", Camera::getViewMatrix());

    for (int i = 0; i < numSpriteSheets; i++){
          glActiveTexture(GL_TEXTURE0 + i);
          spriteSheets[i]->tex->bind();
          shader->uploadTexture(std::string("uTextures[" + std::to_string(i) + "]"), i);

          shader->setVec2("spriteDimensions[" + std::to_string(i) + "]", glm::vec2(spriteSheets[i]->cellWidth, spriteSheets[i]->cellHeight));
          shader->setFloat("cols[" + std::to_string(i) + "]", spriteSheets[i]->numCols);
    }

    templateEBO.bind();
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, numSprites);
    shader->deActivate();

    for (int i = 0; i < numSpriteSheets; i++){
          spriteSheets[i]->tex->unBind();
    }

    VAO.unBind();
    templateEBO.unBind();
    updateDirtyFlags();
}

void SpriteRenderer::addEntity(Terra::Entity& ent){ 
    Component::SpriteRenderer& spr = ent.getComponent<Component::SpriteRenderer>();

    int16_t texID = -1;
    for (int i = 0; i < numSpriteSheets; i++){

        if (&spr.sheet == spriteSheets[i]){
            texID = i;
            break;
        }
    }
    
    if (texID == -1){
        spriteSheets[numSpriteSheets] = &spr.sheet;
        texID = numSpriteSheets++;
    }
   
    index_map[&ent] = numSprites;
    entities[numSprites] = &ent;

    
    Component::Transform& trans = ent.getComponent<Component::Transform>();
    updateTransformData(trans, numSprites);
    updateTexIDData(texID, numSprites);
    updateTexCordData(spr, numSprites);
    updateColorData(spr, numSprites);

    numSprites++;
}


void SpriteRenderer::updateTransformData(Component::Transform& transform, uint32_t index){
    instanceVBO.bind();

    const glm::mat4& trans = transform.getTransform();

    float* buffer_ptr = instanceVBO.mapBuffer<float>();

    uint32_t offset = index * VERTEX_SIZE + (TEXCORD_VERT_SIZE + TEXID_VERT_SIZE + COLOR_VERT_SIZE);
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            int ind = r * 4 + c;
            buffer_ptr[offset + ind] = trans[r][c];
        }
    }

    instanceVBO.unMapBuffer();
    instanceVBO.unBind();
}

void SpriteRenderer::updateColorData(Component::SpriteRenderer& spr, uint32_t index){
    instanceVBO.bind();

    float* buffer_ptr = instanceVBO.mapBuffer<float>();

    uint32_t offset = index * VERTEX_SIZE + (TEXCORD_VERT_SIZE + TEXID_VERT_SIZE);
    buffer_ptr[offset + 0] = spr.color.r;
    buffer_ptr[offset + 1] = spr.color.g;
    buffer_ptr[offset + 2] = spr.color.b;
    buffer_ptr[offset + 3] = spr.color.a;

    instanceVBO.unMapBuffer();
    instanceVBO.unBind();
}

void SpriteRenderer::updateTexCordData(Component::SpriteRenderer& spr, uint32_t index){
    instanceVBO.bind();
    
    float* buffer_ptr = instanceVBO.mapBuffer<float>();

    uint32_t offset = index * VERTEX_SIZE + (0);
    buffer_ptr[offset] = spr.sprite.row * spr.sheet.numCols + spr.sprite.col;

    instanceVBO.unMapBuffer();
    instanceVBO.unBind();
}

void SpriteRenderer::updateTexIDData(int16_t texID, uint32_t index){
    instanceVBO.bind();

    float* buffer_ptr = instanceVBO.mapBuffer<float>();
    uint32_t offset = index * VERTEX_SIZE + (TEXCORD_VERT_SIZE);
    buffer_ptr[offset] = texID;

    instanceVBO.unMapBuffer();
    instanceVBO.unBind();
}

void SpriteRenderer::removeEntity(Terra::Entity& ent){
     uint32_t index = index_map[&ent];

     instanceVBO.bind();
     uint32_t start = index * VERTEX_SIZE;

     float* buffer_ptr = instanceVBO.mapBuffer<float>();
     for (uint32_t i = start; i < numSprites * VERTEX_SIZE - VERTEX_SIZE; i++){
         buffer_ptr[i] = buffer_ptr[i + VERTEX_SIZE];
     }
     instanceVBO.unMapBuffer();
     instanceVBO.unBind();

     
     for (uint32_t i = index; i < numSprites - 1; i++){
         index_map[entities[i + 1]] -= 1;
         entities[i] = entities[i + 1];
     }

     index_map.erase(&ent);
    
     numSprites--;

}

bool SpriteRenderer::containsTexture(const Component::SpriteRenderer& spr){
    for (SpriteSheet* sheet : spriteSheets){
        if (&spr.sheet == sheet)
            return true;
    }
    return false;

}

void SpriteRenderer::updateDirtyFlags(){

    for (uint32_t i = 0; i < numSprites; i++){
        Component::SpriteRenderer& spr = entities[i]->getComponent<Component::SpriteRenderer>();
        if (spr.dirty){
            updateTexCordData(spr, i);
            updateColorData(spr, i);
            spr.dirty = false;
        }

        Component::Transform& trans = entities[i]->getComponent<Component::Transform>();
        if (trans.dirty){

            if (trans.zIndex != zIndex){
                std::cout << "MY ZINDEX " << zIndex << " TRANS ZINDEX " << trans.zIndex << std::endl;
                Terra::Entity& ent = *entities[i];
                renderer.remove(ent);
                renderer.submit(ent);
            }
            else
                updateTransformData(trans, i);

            trans.dirty = false;
        }
    }
}