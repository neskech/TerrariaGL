#include "renderer/VAO.h"
#include "glm/glm.hpp"

VertexArrayObject::VertexArrayObject(int vertexSizeBytes_): vertexSizeBytes(vertexSizeBytes_),
numAttributes(0), byteLength(0)
{
    glGenVertexArrays(1, &ID);
}

VertexArrayObject::~VertexArrayObject(){
    glDeleteVertexArrays(1, &ID);
}


void VertexArrayObject::addAtribute(int stride, int dataType, int typeSize, int divisor){
    glVertexAttribPointer(numAttributes, stride, dataType, GL_FALSE, vertexSizeBytes, (void*)byteLength);
    glEnableVertexAttribArray(numAttributes);  

    if (divisor >= 0)
        glVertexAttribDivisor(numAttributes, divisor);

    numAttributes++;
    byteLength += stride * typeSize;

}

void VertexArrayObject::addMatrixAttribute(int rows, int cols, int divisor){

    std::size_t size = 0;
    switch (cols){
        case 2:
            size = sizeof(glm::vec2);
            break;
        case 3:
             size = sizeof(glm::vec3);
            break;
        case 4:
             size = sizeof(glm::vec4);
            break;
        default:
            std::cerr << "ERROR: In VertexArrayObject::addMatrixAttribute : " << cols << " is an invalid size\n";
            return; 
    }

    for (int i = 0; i  < rows; i++){
        glEnableVertexAttribArray(numAttributes);  
        glVertexAttribPointer(numAttributes, cols, GL_FLOAT, GL_FALSE, rows * size, (void*)byteLength);

        if (divisor >= 0)
             glVertexAttribDivisor(numAttributes, divisor);

        numAttributes++;
        byteLength += size;
    }

}