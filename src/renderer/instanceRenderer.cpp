#include "renderer/instanceRenderer.h"
#include "renderer/renderer.h"
#include "scene/camera.h"

static constexpr int TRANSFORM_VBO_VERT_SIZE = 16;
static constexpr int UV_VBO_VERT_SIZE = 2;
static constexpr int TEXID_VBO_VERT_SIZE = 1;

InstanceRenderer::InstanceRenderer():
    instance_transform_VBO(BufferType::VBO), instance_uv_VBO(BufferType::VBO), instance_texID_VBO(BufferType::VBO), templateVBO(BufferType::VBO),
    EBO(BufferType::EBO), VAO(sizeof(float) * 2), numSprites(0), numTextures(0)
{
    index_map.reserve(MAX_INSTANCES);
}

InstanceRenderer::~InstanceRenderer(){
}

void InstanceRenderer::init(){
    shader = AssetPool::getShader("../assets/shaders/test.glsl");
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
    
    EBO.bind();
    EBO.addData(indices, 6, 0, GL_STATIC_DRAW);

    templateVBO.bind();
    templateVBO.addData(quad, 8, 0, GL_STATIC_DRAW);

    //Pos -- template QUAD
    VAO.addAtribute(2, GL_FLOAT, sizeof(float));

    instance_uv_VBO.bind();
    instance_uv_VBO.allocateData<float>(MAX_INSTANCES * UV_VBO_VERT_SIZE);

    //UVs -- passed every vertex
    VAO.resetByteCount();
    VAO.addAtribute(2, GL_FLOAT, sizeof(float), 0);

    instance_texID_VBO.bind();
    instance_texID_VBO.allocateData<float>(MAX_INSTANCES * TEXID_VBO_VERT_SIZE);

    //TexID -- passed every instance
    VAO.resetByteCount();
    VAO.addAtribute(1, GL_FLOAT, sizeof(float), 1);

    instance_transform_VBO.bind();
    instance_transform_VBO.allocateData<float>(MAX_INSTANCES * TRANSFORM_VBO_VERT_SIZE);

    //Transform -- passed every instance
    VAO.resetByteCount();
    VAO.addMatrixAttribute(4, 4, 1);

    templateVBO.unBind();
    instance_transform_VBO.unBind();
    instance_uv_VBO.unBind();
    instance_texID_VBO.unBind();
    EBO.unBind();
    VAO.unBind();
}


void InstanceRenderer::render(){
    VAO.bind();

    shader->activate();
    shader->setmat4("ortho", Camera::getProjectionMatrix());
    shader->setmat4("view", Camera::getViewMatrix());

    for (int i = 0; i < numTextures; i++){
          glActiveTexture(GL_TEXTURE0 + i);
          textures[i]->bind();
          shader->uploadTexture(std::string("uTextures"), i);
    }

    EBO.bind();
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, numSprites);
    shader->deActivate();

    for (int i = 0; i < numTextures; i++){
          textures[i]->unBind();
    }

    VAO.unBind();
    updateDirtyFlags();
}

void InstanceRenderer::addEntity(Terra::Entity& ent){ 
    Component::SpriteRenderer& spr = ent.getComponent<Component::SpriteRenderer>();

    int16_t texID = -1;
    for (int i = 0; i < numTextures; i++){

        if (spr.sheet.tex.get() == textures[i]){
            texID = i;
            break;
        }
    }
    
    if (texID == -1){
        textures[numTextures] = spr.sheet.tex.get();
        texID = numTextures++;
    }
   
    index_map[&ent] = numSprites;
    entities[numSprites] = &ent;
    
    Component::Transform& trans = ent.getComponent<Component::Transform>();
    updateTransformData(trans, numSprites);
    updateTexIDData(texID, numSprites);
    updateUVData(spr, numSprites);

    numSprites++;

}


void InstanceRenderer::updateTransformData(Component::Transform& transform, uint32_t index) const{
    instance_transform_VBO.bind();
    //transform
    const glm::mat4& trans = transform.getTransform();

    float* buffer_ptr = instance_transform_VBO.mapBuffer<float>();

    uint32_t offset = index * TRANSFORM_VBO_VERT_SIZE;
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            int index = r * 4 + c;
            buffer_ptr[offset + index] = trans[r][c];
        }
    }

    instance_transform_VBO.unMapBuffer();
    instance_transform_VBO.unBind();
}

void InstanceRenderer::updateUVData(Component::SpriteRenderer& spr, uint32_t index) const{
    instance_uv_VBO.bind();
    
    float* buffer_ptr = instance_uv_VBO.mapBuffer<float>();

    glm::vec2 uvs(spr.sheet.cellWidth * spr.sprite.col, spr.sheet.cellHeight * spr.sprite.row);

    uint32_t row, col;
    uint32_t offset = index * UV_VBO_VERT_SIZE * 4;
    for (int i = 0; i < 4; i++){
        row = i / 2;
        col = i % 2;

        buffer_ptr[offset + i * UV_VBO_VERT_SIZE + 0] = (uvs.x + spr.sheet.cellWidth * col) / spr.sheet.tex->getWidth();
        buffer_ptr[offset + i * UV_VBO_VERT_SIZE + 1] =  (uvs.y + spr.sheet.cellHeight * row) / spr.sheet.tex->getHeight();

    }

    instance_uv_VBO.unMapBuffer();
    instance_uv_VBO.unBind();
}

void InstanceRenderer::updateTexIDData(int16_t texID, uint32_t index) const{
    instance_texID_VBO.bind();

    float* buffer_ptr = instance_texID_VBO.mapBuffer<float>();
    uint32_t offset = index * UV_VBO_VERT_SIZE;
    buffer_ptr[offset] = texID;

    instance_texID_VBO.unBind();
    instance_texID_VBO.unMapBuffer();
}

void InstanceRenderer::removeEntity(Terra::Entity& ent){
     uint32_t index = index_map[&ent];

     instance_transform_VBO.bind();
     uint32_t start = index * TRANSFORM_VBO_VERT_SIZE;

     float* buffer_ptr = instance_transform_VBO.mapBuffer<float>();
     for (uint32_t i = start; i < numSprites * TRANSFORM_VBO_VERT_SIZE - TRANSFORM_VBO_VERT_SIZE; i++){
         buffer_ptr[i] = buffer_ptr[i + TRANSFORM_VBO_VERT_SIZE];
     }
     instance_transform_VBO.unMapBuffer();
     instance_transform_VBO.unBind();
    
     instance_uv_VBO.bind();
     start = index * UV_VBO_VERT_SIZE * 4;
     buffer_ptr = instance_uv_VBO.mapBuffer<float>();
     for (uint32_t i = start; i < numSprites * UV_VBO_VERT_SIZE * 4 - UV_VBO_VERT_SIZE * 4; i++){
         buffer_ptr[i] = buffer_ptr[i + UV_VBO_VERT_SIZE * 4];
     }
     instance_uv_VBO.unMapBuffer();
     instance_uv_VBO.unBind();

     instance_texID_VBO.bind();
     start = index * TEXID_VBO_VERT_SIZE;
     buffer_ptr = instance_texID_VBO.mapBuffer<float>();
     for (uint32_t i = start; i < numSprites * TEXID_VBO_VERT_SIZE - TEXID_VBO_VERT_SIZE; i++){
         buffer_ptr[i] = buffer_ptr[i + TEXID_VBO_VERT_SIZE];
     }
     instance_texID_VBO.unMapBuffer();
     instance_texID_VBO.unBind();

     for (uint32_t i = index; i < numSprites - 1; i++){
         index_map[entities[i + 1]] -= 1;
         entities[i] = entities[i + 1];
     }

     numSprites--;

}

void InstanceRenderer::updateDirtyFlags(){

    for (uint32_t i = 0; i < numSprites; i++){
        Component::SpriteRenderer& spr = entities[i]->getComponent<Component::SpriteRenderer>();
        if (spr.dirty){
            updateUVData(spr, i);
            spr.dirty = false;
        }

        Component::Transform& trans = entities[i]->getComponent<Component::Transform>();
        if (trans.dirty){
            updateTransformData(trans, i);
            trans.dirty = false;
        }
    }
}

