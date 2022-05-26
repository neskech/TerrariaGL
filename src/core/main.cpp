#include <iostream>
#include "core/application.h"

int main(){

    Application* app = new Application(800, 800);
    app->init();
    app->run();
    delete app;
    return 0;
}

