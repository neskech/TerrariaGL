#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera* Camera::instance = nullptr;

Camera::Camera(glm::vec3 pos_, glm::vec3 worldUp_){
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
    return glm::lookAt(instance->pos, instance->pos + instance->forward, instance->up);
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