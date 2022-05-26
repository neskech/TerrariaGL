#include "renderer/buffer.h"

Buffer::Buffer(BufferType bufferType_)
{   
    bufferType = static_cast<int>(bufferType_);
    glGenBuffers(bufferType, &ID);
}

Buffer::~Buffer(){
    glDeleteBuffers(1, &ID);
}

template <class T>
void Buffer::addData(T* elements, int length_, int offset, int drawType){
    length = length_;

    bind();
    glBufferData(bufferType, length * sizeof(T), &elements[offset], drawType);
    unBind();
}

template <class T>
void Buffer::addSubData(T* elements, int length, int offset, int drawType){
    bind();
    glBufferSubData(bufferType, offset, length, &elements[offset]);
    unBind();
}

template <class T>
void Buffer::allocateData(int length){
    bind();
    glBufferData(bufferType, length * sizeof(T), NULL, GL_STATIC_DRAW);
    unbind();
}