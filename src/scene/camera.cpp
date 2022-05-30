#include "scene/camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera* Camera::instance = nullptr;

Camera::Camera(const glm::vec3& pos_, const glm::vec3& worldUp_, const glm::vec3& forward_): pos(pos_), worldUp(worldUp_), forward(forward_){
     if (instance != nullptr)
        assert(false);

     instance = this;
     updateVectors();
}

Camera::~Camera(){

}

glm::mat4 Camera::getProjectionMatrix() { 
    return glm::ortho(-CAM_WIDTH / 2.0f, CAM_WIDTH / 2.0f, -CAM_HEIGHT / 2.0f, CAM_HEIGHT / 2.0f, 0.1f, 100.0f);
}

glm::mat4 Camera::getViewMatrix() { 
    return glm::lookAt(instance->pos, instance->pos + instance->forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::changeLookDirection(const glm::vec3& look){ 
    instance->forward = look; 
    updateVectors(); 
}

void Camera::changePosition(const glm::vec3& position){ 
    instance->pos = position; 
    updateVectors(); 
}

void Camera::updateVectors(){
    instance->right = glm::normalize(glm::cross(instance->up, instance->forward));
    instance->up = glm::cross(instance->forward, instance->right);
}