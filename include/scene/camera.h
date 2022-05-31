#pragma once
#include "pch.h"

constexpr float CAM_WIDTH = 128;
constexpr float CAM_HEIGHT = 100;

class Camera{
    public:
        Camera(const glm::vec2& pos_ = glm::vec2(0.0f, 0.0f));
        ~Camera();
        

        static glm::mat4 getProjectionMatrix();
        static glm::mat4 getViewMatrix();
        static void changePosition(const glm::vec2& position);

        static inline glm::vec2 getPosition(){ return instance->pos; }
    private:
        glm::vec2 pos;
        static Camera* instance;
};
