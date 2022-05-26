#pragma once
#include <glad/glad.h>

class VertexArrayObject{
    public:
        VertexArrayObject();
        VertexArrayObject(const VertexArrayObject& other) = delete;
        VertexArrayObject(VertexArrayObject& other) = delete;
        VertexArrayObject(VertexArrayObject&& other) = delete;
        ~VertexArrayObject();

        void addAtribute(int stride, int dataType, int typeSize, bool divisor = false);
        void addMatrixAttribute(int rows, int cols, bool divisor = false);

        inline void bind(){ glBindVertexArray(ID); }
        inline void unBind(){ glBindVertexArray(0); }
    private:
        unsigned int ID;
        uint16_t numAttributes;
        int byteLength;
};