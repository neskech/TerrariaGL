#pragma once
#include "components/component.h"
#include "renderer/instanceRenderer.h"
#include <vector>

class Renderer{
    public:
        Renderer();
        Renderer(Renderer& other) = delete;
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        ~Renderer();

        static void render();
        static void init();
        static void remove(Component::SpriteRenderer& spr);
        static void submit(Component::SpriteRenderer& spr);

    private:
        InstanceRenderer* instanceRenderer;
        static Renderer* instance;
        friend class InstanceRenderer;

};