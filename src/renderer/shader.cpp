#include "renderer/shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "glad/glad.h"

Shader::Shader(const char* path): 
shaderPath(path)
{

}

Shader::~Shader(){

}

bool Shader::readShaders(std::string& vs, std::string& fs){
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

bool Shader::compile(){
    std::string vertexShader, fragmentShader;
    if (!readShaders(vertexShader, fragmentShader)){
        std::cerr << "ERROR: In shader::compile. Failed to read shaders from " << shaderPath << '\n';
        return false;
    }

    int vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, vertexShader.str, NULL);
    glCompileShader(vShader);

    int fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vShader, 1, vertexShader.c_str(), NULL);
    glCompileShader(fShader);

    programID = glCreateProgram();
    glAttachShader(programID, vShader);
    glAttachShader(programID, fShader);
    glLinkProgram(programID);

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

void Shader::activate(){
    glUseProgram(programID);
}

void Shader::deActivate(){
    glUseProgram(0);
}