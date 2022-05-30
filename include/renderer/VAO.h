#pragma once
#include "pch.h"

class VertexArrayObject{
    public:
        VertexArrayObject(int vertexSizeBytes_);
        VertexArrayObject(const VertexArrayObject& other) = delete;
        VertexArrayObject(VertexArrayObject& other) = delete;
        VertexArrayObject(VertexArrayObject&& other) = delete;
        ~VertexArrayObject();

        void addAtribute(int stride, int dataType, int typeSize, int divisor = -1);
        void addMatrixAttribute(int rows, int cols, bool divisor = false);

        inline void bind(){ glBindVertexArray(ID); }
        inline void unBind(){ glBindVertexArray(0); }
        inline void resetByteCount(){ byteLength = 0; }
    private:
        unsigned int ID;
        int vertexSizeBytes;
        int numAttributes;
        int byteLength;
};