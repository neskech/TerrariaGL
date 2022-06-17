#pragma once
#include "window.h"
#include "core/input.h"
#include "util/assetPool.h"
#include "scene/scene.h"

//Simply used to display the FPS
struct SettingsInfo{

        bool display;
        float elapsedTimeBeforeHiding;
        float hideDelay;

        float refreshRate;
        float elapsedTimeBeforeRefresh;
        int FPS;

        SettingsInfo(): display(true), elapsedTimeBeforeHiding(0.0f), hideDelay(0.5f), refreshRate(0.5f),
                        elapsedTimeBeforeRefresh(0.0f), FPS(0) {}
};

class Application{

    public:
        Application(uint32_t windowWidth, uint32_t windowHeight);
        ~Application();
        Application(Application& other) = delete;
        Application(const Application& other) = delete;
        Application(Application&& other) = delete;

        bool init();
        void run();

        inline Application* getInstance(){ return instance; }
        inline Window* getWindow(){ return window; }

    private:
        void displaySettingsAndInfo(float delta);
        static Application* instance;
        
        Scene* scene;
        Window* window;
        KeyListener* keyListener;
        MouseListener* mouseListener;
        AssetPool* assetPool;

        SettingsInfo settings;
};