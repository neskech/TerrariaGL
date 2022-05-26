#pragma once
#include <glad/glad.h>
#include <iostream>

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
            std::cout << "HEHEHHEHE\n";
            bufferType = to_underlying(bufferType_);
            std::cout << bufferType << " " << GL_ARRAY_BUFFER << '\n';
            glGenBuffers(bufferType, &ID);
            std::cout << "JHADHIBWDYABIDWBAW\n";
        }

        ~Buffer(){
            glDeleteBuffers(1, &ID);
        }

        Buffer(Buffer& other) = delete;
        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) = delete;
        
        template <class T>
        void addData(T* elements, int length_, int offset, int drawType){
            length = length_;

            bind();
            glBufferData(bufferType, length * sizeof(T), &elements[offset], drawType);
            unBind();
        }

        template <class T>
        void addSubData(T* elements, int length, int offset, int drawType){
            bind();
            glBufferSubData(bufferType, offset, length, &elements[offset]);
            unBind();
        }

        template <class T>
        void allocateData(int length){
            bind();
            glBufferData(bufferType, length * sizeof(T), nullptr, GL_STATIC_DRAW);
            unBind();
        }

        template <class T>
        T* mapBuffer(){ return (T*)glMapBuffer(bufferType, GL_WRITE_ONLY); }
        bool unMapBuffer(){ return glUnmapBuffer(bufferType); }


        inline void bind(){ glBindBuffer(bufferType, ID); }
        inline void unBind(){ glBindBuffer(bufferType, 0); }
        bool isValid(){ return ID != 0; }

        inline BufferType getBufferType() const{ return static_cast<BufferType>(bufferType); }
        inline uint32_t getLength() const{ return length; }
        inline unsigned int getID() const{ return ID; }
    private:
        unsigned int ID;
        int bufferType;
        uint32_t length;

};