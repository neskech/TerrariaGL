#include "renderer/framebuffer.h"
#include "renderer/texture.h"
#include "core/window.h"

FrameBuffer::FrameBuffer(){
    glGenFramebuffers(1, &ID);

}
FrameBuffer::~FrameBuffer(){
    glDeleteFramebuffers(1, &ID);  
}

bool FrameBuffer::create(){
    bind();

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::getWidth(), Window::getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0); 

    glGenRenderbuffers(1, &rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);  
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::getWidth(), Window::getHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);  

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);  

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
	    std::cout << "ERROR::In FrameBuffer | Could not create framebuffer!" << std::endl;
        return false;
    }

    unBind();
    return true; 
}

void FrameBuffer::enableGLSettings(){      
    glEnable(GL_BLEND);  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

 void FrameBuffer::clearColor(float r, float g, float b, float a){
     glClearColor(r, g, b, a);
 }

void FrameBuffer::clearType(int clearType){
    glClear(clearType);
}
