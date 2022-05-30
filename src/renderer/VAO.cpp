#include "renderer/VAO.h"
#include "glm/glm.hpp"

VertexArrayObject::VertexArrayObject(int vertexSizeBytes_): vertexSizeBytes(vertexSizeBytes_){
    glGenVertexArrays(1, &ID);
    numAttributes = 0;
    byteLength = 0;
}

VertexArrayObject::~VertexArrayObject(){
    glDeleteVertexArrays(1, &ID);
}


void VertexArrayObject::addAtribute(int stride, int dataType, int typeSize, int divisor){
    std::cout << "numAttrs " << numAttributes << " byteLength " << byteLength << "\n";
    glVertexAttribPointer(numAttributes, stride, dataType, GL_FALSE, vertexSizeBytes, (void*)byteLength);
    glEnableVertexAttribArray(numAttributes);  

    if (divisor >= 0)
        glVertexAttribDivisor(numAttributes, divisor);

    numAttributes++;
    byteLength += stride * typeSize;

}

void VertexArrayObject::addMatrixAttribute(int rows, int cols, bool divisor){

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

        if (divisor)
             glVertexAttribDivisor(numAttributes, 1);

        numAttributes++;
        byteLength += size;
    }

}