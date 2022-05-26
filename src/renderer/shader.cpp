#include "renderer/shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "glad/glad.h"

Shader::Shader(const char* shaderPath){
    std::cout<<"Made shader!\n";
    if (!construct(shaderPath))
        std::cerr << "ERROR : In shader constructor. Shader creation failed!\n";
}

Shader::~Shader(){

}

bool Shader::readShaders(const char* shaderPath, std::string& vs, std::string& fs){
    std::ifstream file(shaderPath);
    if (!file.is_open()){
        std::cerr << "ERROR: In Shader::readShaders. Unable to open the file -- " << shaderPath << " --\n";
        return false;
    }

    std::stringstream streams[2];
    std::string line;
    int a = 0;
    while (file.good()){
        std::getline(file, line);
        if (line.find("#shader") != std::string::npos){
            if (line.find("vertex") != std::string::npos)
                a = 0;
            else if (line.find("fragment") != std::string::npos)
                a = 1;
            else{
                std::cerr << "ERROR: In Shader::readShaders. No shader identifier (vertex, fragment, etc) found next to the #shader tag! Path: " << shaderPath << '\n';
                return false;
            }

        }
        else
            streams[a] << line << '\n';
    }

    vs = streams[0].str();
    fs = streams[1].str();

}

bool Shader::construct(const char* shaderPath){
    std::string vertexShader, fragmentShader;
    if (!readShaders(shaderPath, vertexShader, fragmentShader)){
        std::cerr << "ERROR: In shader::compile. Failed to read shaders from " << shaderPath << '\n';
        return false;
    }

    int vShader = createShader(vertexShader, GL_VERTEX_SHADER);
    glCompileShader(vShader);

    int fShader = createShader(fragmentShader, GL_FRAGMENT_SHADER);
    glCompileShader(fShader);

    programID = createProgram(vShader, fShader);

    //check success status
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if (!success){
        char buffer[512];
        glGetProgramInfoLog(programID, 512, NULL, buffer);

        std::cerr << "ERROR: In shader::compile. Failed to link shaders from " << shaderPath << '\n'
        << buffer << '\n';
        return false;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return true;

}

int Shader::createShader(std::string& shaderSrc, int shaderType){
    int shaderComponent = glCreateShader(shaderType);
    const char* src = shaderSrc.c_str();
    glShaderSource(shaderComponent, 1, &src, NULL);
    return shaderComponent;
}

int Shader::createProgram(int vShader, int fShader){
    int ID = glCreateProgram();
    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);
    return ID;
}

void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value); 
} 
void Shader::setVec2(const std::string &name, glm::vec2 vec) const
{ 
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 2,  (GLfloat*) &vec); 
} 
void Shader::setVec3(const std::string &name, glm::vec3 vec) const
{ 
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 3,  (GLfloat*) &vec); 
} 
void Shader::setVec4(const std::string &name, glm::vec4 vec) const
{ 
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 4,  (GLfloat*) &vec); 
} 
void Shader::setmat3(const std::string &name, glm::mat3x3 mat) const
{ 
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]); 
} 
void Shader::setmat4(const std::string &name, glm::mat4x4 mat) const
{ 
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]); 
} 

void Shader::use(){
    glUseProgram(programID);
}

void Shader::deActivate(){
    glUseProgram(0);
}