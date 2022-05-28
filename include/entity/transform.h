#pragma once
#include "pch.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct Transform{
    glm::vec2 translation;
    glm::vec2 rotation;
    glm::vec2 scale;

    Transform(glm::vec3& translation_): translation(translation_) {}
    Transform() = default;

    glm::mat4 getTransform(){
        return  glm::translate(glm::mat4(1.0f), glm::vec3(translation, 0.0f))) 
        * glm::toMat4(glm::quat(glm::vec3(rotation, 0.0f))) 
        * glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f)));
    }
}
