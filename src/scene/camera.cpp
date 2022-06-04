#include "scene/camera.h"
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>

Camera* Camera::instance = nullptr;

Camera::Camera(const glm::vec2& pos_): pos(pos_){
     if (instance != nullptr)
        assert(false);

     instance = this;
}

Camera::~Camera(){

}

glm::mat4 Camera::getProjectionMatrix() { 
    return glm::ortho(-CAM_WIDTH / 2.0f, CAM_WIDTH / 2.0f, -CAM_HEIGHT / 2.0f, CAM_HEIGHT / 2.0f, 0.1f, 100.0f);
}

glm::mat4 Camera::getViewMatrix() { 
    glm::vec3 tempPos = glm::vec3(instance->pos, 10.0f);
    return glm::lookAt(tempPos, tempPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::changePosition(const glm::vec2& position){ 
    instance->pos.y= CLAMP(position.y, -CHUNK_HEIGHT / 2 + CAM_HEIGHT + 1, CHUNK_HEIGHT / 2 - CAM_HEIGHT);
    instance->pos.x = position.x; 
}
