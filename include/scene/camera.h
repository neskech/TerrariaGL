#pragma once
#include "pch.h"

constexpr int CAM_WIDTH = 128;
constexpr int CAM_HEIGHT = 100;

class Camera{
    public:
        Camera(glm::vec3 pos_ = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp_ = glm::vec3(0.0f, 1.0f, 0.0f));
        ~Camera();
        

        glm::mat4 getProjectionMatrix();
        glm::mat4 getViewMatrix();

        void changeLookDirection(glm::vec3 look);
        void changePosition(glm::vec3 position);

        inline glm::vec3 getPosition(){ return pos; }
        inline glm::vec3 getForwardDirection(){ return forward; }
    private:
        glm::vec3 pos;
        glm::vec3 worldUp;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 forward;

        void updateVectors();
};