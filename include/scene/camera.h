#pragma once
#include "pch.h"
#include "constants.h"


class Camera{
    public:
        Camera(const glm::vec2& pos_ = glm::vec2(0.0f, 0.0f));
        ~Camera(){}
        

        static glm::mat4 getProjectionMatrix();
        static glm::mat4 getViewMatrix();
        static void changePosition(const glm::vec2& position);

        static inline glm::vec2 getPosition(){ return instance->pos; }
        static inline int getCamViewWidth(){ return CAM_WIDTH; }
        static inline int getCamViewHeight(){ return CAM_HEIGHT; } 
        static inline bool isDirty(){ return instance->dirty; }
        static inline void unDirty(){ instance->dirty = false; }
    private:
        glm::vec2 pos;
        bool dirty;
        static Camera* instance;
};
