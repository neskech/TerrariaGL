#pragma once
#include <glad/glad.h>

enum class BufferType{
    VBO = GL_ARRAY_BUFFER,
    EBO = GL_ELEMENT_ARRAY_BUFFER
};


class Buffer{
    public:
        Buffer(BufferType type);
        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) = delete;
        ~Buffer();
        
        template <class T>
        void addData(T* elements, int length, int offset, int drawType);

        template <class T>
        void addSubData(T* elements, int length, int offset, int drawType);

        template <class T>
        void allocateData(int length);

        template <class T>
        T* mapBuffer(){ return (T*)glMapBuffer(bufferType, GL_WRITE_ONLY); }
        bool unMapBuffer(){ return glUnmapBuffer(bufferType); }


        inline void bind(){ glBindBuffer(bufferType, ID); }
        inline void ubBind(){ glBindBuffer(bufferType, 0); }
        bool isValid(){ return ID != 0; }

        inline BufferType getBufferType() const{ return static_cast<BufferType>(bufferType); }
        inline uint32_t getLength() const{ return length; }
        inline unsigned int getID() const{ return ID; }
    private:
        unsigned int ID;
        int bufferType;
        uint32_t length;
};