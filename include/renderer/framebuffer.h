#pragma once
#include "pch.h"

class FrameBuffer{
    public:
        FrameBuffer();
        FrameBuffer(FrameBuffer& other) = delete;
        FrameBuffer(const FrameBuffer& other) = delete;
        FrameBuffer(FrameBuffer&& other) = delete;
        ~FrameBuffer();

        bool create();
        void enableGLSettings();
        void clearColor(float r, float g, float b, float a);
        void clearType(int clearType);

        inline void bind(){ glBindFramebuffer(GL_FRAMEBUFFER, ID); }
        inline void unBind(){ glBindFramebuffer(GL_FRAMEBUFFER, 0); }
        inline unsigned int textureColorAttachment(){ return textureID; }
        inline unsigned int depthAndStencilAttachment(){ return rboID; }

    private:
        unsigned int ID;
        unsigned int textureID;
        unsigned int rboID;
};