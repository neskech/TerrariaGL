#pragma once
#include "pch.h"

enum class BufferType{
    VBO = GL_ARRAY_BUFFER,
    EBO = GL_ELEMENT_ARRAY_BUFFER
};

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

class Buffer{
    public:
        Buffer(BufferType bufferType_)
        {   
            bufferType = to_underlying(bufferType_);
            glGenBuffers(1, &ID);
        }

        ~Buffer(){
            glDeleteBuffers(1, &ID);
        }

        Buffer() = delete;
        Buffer(Buffer& other) = delete;
        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) = delete;
        
        template <class T>
        void addData(T* elements, int length_, int offset, int drawType){
            length = length_;

            glBufferData(bufferType, length * sizeof(T), &elements[offset], drawType);
        }

        template <class T>
        void addSubData(T* elements, int length, int offset){
            glBufferSubData(bufferType, offset, length, &elements[offset]);
        }

        template <class T>
        void allocateData(int length, int drawType){
            glBufferData(bufferType, length * sizeof(T), NULL, drawType);
        }

        template <class T>
        T* mapBuffer() const { return (T*)glMapBuffer(bufferType, GL_WRITE_ONLY); }
        bool unMapBuffer() const{ return glUnmapBuffer(bufferType); }


        inline void bind() const { glBindBuffer(bufferType, ID); }
        inline void unBind() const { glBindBuffer(bufferType, 0); }
        bool isValid() const{ return ID != 0; }

        inline BufferType getBufferType() { return static_cast<BufferType>(bufferType); }
        inline int getLength(){ return length; }
        inline unsigned int getID(){ return ID; }
    private:
        unsigned int ID;
        int bufferType;
        int length;

};