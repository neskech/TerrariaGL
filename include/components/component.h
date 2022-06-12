#pragma once
#include "scene/entity.h"
#include "components/spriteInfo.h"
#include "components/animation.h"
#include "scene/scene.h"
#include <functional>
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

    struct physicsBody{
        glm::vec2 velocity;
        glm::vec2 accerlation;
        float mass;

        physicsBody(float mass_ = 1.0f): mass(mass_) {}

        void addForce(const glm::vec2& force){ accerlation += force / mass; }
    };

    struct AABB{
        float extentsX;
        float extentsY;

        AABB(float extentsX_, float extentsY_): extentsX(extentsX_), extentsY(extentsY_) {}
    };

    struct SpriteRenderer{
        SpriteSheet sheet;
        Sprite sprite;
        glm::vec4 color;

        bool dirty;

        SpriteRenderer(const SpriteSheet& sheet_, Sprite spr_ = {0, 0}) : sheet(sheet_), sprite(spr_) {}
        
        void changeColor(const glm::vec4& c){ color = c; dirty = true; }

        void changeSprite(Sprite spr){ sprite = spr; dirty = true; }
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

            const Animation& getCurrentAnimation(){
                return animations[currentAnimation];
            }

            void advance(float timeStep){
                animations[currentAnimation].advance(timeStep);
            }

        private:
            ANIMATION_TYPE currentAnimation;
            std::unordered_map<ANIMATION_TYPE, Animation> animations;
    };

    struct CollisionData{
        glm::vec2 pos;
        Component::AABB collider;

        CollisionData(const glm::vec2& pos_, const Component::AABB& collider_): pos(pos_), collider(collider_) {}
    };

    class ScriptableObject{
        public:
            ScriptableObject(Terra::Entity& entity_, Scene* scene_): entity(entity_), scene(scene_) {}
            virtual void start(){}
            virtual void update(float timeStep){}
            virtual void onDestroy(){}
            virtual void onCollision(const CollisionData& collision){}
            virtual ~ScriptableObject(){}
        protected:
            Terra::Entity& entity;
            Scene* scene;
    };

    //When accesing all scripts via a view<> from the registry, each script must share the same type
    //Cannot use the ScriptableObject base class to accomplish this, so using this instead
    struct Script{
        ScriptableObject* script;
        std::function<void(void)> start;
        std::function<void(float)> update;
        std::function<void(void)> onDestroy;
        std::function<void(const CollisionData& collision)> onCollision;

        Script(ScriptableObject* derivedScript): script(derivedScript)
        {
            using namespace std::placeholders;
            start = std::bind(&ScriptableObject::start, script);
            update = std::bind(&ScriptableObject::update, script, std::placeholders::_1);
            onDestroy = std::bind(&ScriptableObject::onDestroy, script);
            onCollision = std::bind(&ScriptableObject::onCollision, script, std::placeholders::_1);
        }

        ~Script(){std::cout<<"It was fun!\n";}
    };
}