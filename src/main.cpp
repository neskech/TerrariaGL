#include <iostream>
#include "window.h"
#include "input/input.h"

static int* get_bits(int n, int bitswanted){
  int* bits = (int*) malloc(sizeof((int)bitswanted));

  int k;
  for(k=0; k<bitswanted; k++){
    int mask =  1 << k;
    int masked_n = n & mask;
    int thebit = masked_n >> k;
    bits[k] = thebit;
  }

  return bits;
}

int main(){

    Window* window = new Window(800, 800);
    if (!window->init())
        exit(1);
    
    MouseListener* m = new MouseListener(window);
    KeyListener* k = new KeyListener();

    if ( !MouseListener::setCursorImage("../assets/abc.png") ){
        std::cerr << "Could not make the cursor Image!\n";
        delete window;
        exit(1);
    }


    while (!window->windowClosing()){
         if (MouseListener::isMouseButtonPressed(GLFW_MOUSE_BUTTON_1)){
            std::cout << MouseListener::isMouseButtonModdedBy(GLFW_MOUSE_BUTTON_1, GLFW_MOD_CONTROL) << " \n";
         }
         window->update();
    }

    delete m;
    delete k;
    delete window;
    return 0;
}

