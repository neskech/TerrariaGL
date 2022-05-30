#pragma once
#include "scene/entity.h"
#include "components/spriteInfo.h"
#include "components/animation.h"
#include <unordered_map>
#include <string>


#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


enum ANIMATION_TYPE{
    WALK_LEFT,
    WALK_RIGHT,
    JUMP,
    ATTACK
};

namespace Component{
    
    struct Tag{
        std::string name;
        Tag(std::string&& name_): name(name_){}
    };

    struct Transform{
        glm::vec2 position;
        glm::vec2 rotation;
        glm::vec2 scale;
        bool dirty;

        Transform(glm::vec2& translation_): position(translation_) {}
        Transform() = default;

        glm::mat4 getTransform(){
            return  glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) 
            * glm::toMat4(glm::quat(glm::vec3(rotation, 0.0f))) 
            * glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));
        }

        void setPos(const glm::vec2& pos){ position = pos; dirty = true; }
        void setRot(const glm::vec2& rot){ rotation = rot; dirty = true;}
        void setScale(const glm::vec2& sc){ scale = sc; dirty = true; }
    };

    struct SpriteRenderer{
        SpriteSheet sheet;
        Sprite sprite;

        glm::vec3 color;
        Terra::Entity parent;
        void (*dirtyCallback)(SpriteRenderer* spr);
        uint32_t renderer_index; //very very messy solution ):

        SpriteRenderer(const Terra::Entity& ent_, const SpriteSheet& sheet_, Sprite spr_ = {0, 0})
        : parent(ent_), sheet(sheet_), sprite(spr_) {}
        
        void changeColor(const glm::vec3& c){ color = c; dirty(); }

        void changeSprite(Sprite spr){ sprite = spr; dirty(); }

        void dirty(){ 
            if (dirtyCallback != nullptr) 
                dirtyCallback(this);
        }
    };

    //keep track of the current animation
    //updating current animation via advance() method
    //possible state machine behavior for switching animations
    class AnimationController{
        public:
            AnimationController(){
                animations.reserve(1);
            }
            ~AnimationController(){}

            void addAnimation(ANIMATION_TYPE aniType, const Animation& ani){
                animations[aniType] = ani;
            }

            void setCurrentAnimation(ANIMATION_TYPE aniType){
                currentAnimation = aniType;
            }

            void advance(float timeStep){
                animations[currentAnimation].advance(timeStep);
            }

        private:
            ANIMATION_TYPE currentAnimation;
            std::unordered_map<ANIMATION_TYPE, Animation> animations;
    };

    class ScriptableObject{
        public:
            virtual void start();
            virtual void update(float timeStep);
            virtual void destroy();
    };

}