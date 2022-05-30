#include "scene/camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera* Camera::instance = nullptr;

Camera::Camera(const glm::vec3& pos_): pos(pos_){
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
    return glm::lookAt(instance->pos, instance->pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::changePosition(const glm::vec3& position){ 
    instance->pos = position; 
    updateVectors(); 
}
