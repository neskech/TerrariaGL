#pragma once
#include "pch.h"

constexpr float CAM_WIDTH = 128;
constexpr float CAM_HEIGHT = 100;

class Camera{
    public:
        Camera(glm::vec3 pos_ = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp_ = glm::vec3(0.0f, 1.0f, 0.0f));
        ~Camera();
        

        static glm::mat4 getProjectionMatrix();
        static glm::mat4 getViewMatrix();
        static void changeLookDirection(const glm::vec3& look);
        static void changePosition(const glm::vec3& position);

        static void updateVectors();

        static inline glm::vec3 getPosition(){ return instance->pos; }
        static inline glm::vec3 getForwardDirection(){ return instance->forward; }
    private:
        glm::vec3 pos;
        glm::vec3 worldUp;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 forward;

        static Camera* instance;
};
