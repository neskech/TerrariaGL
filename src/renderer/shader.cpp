#include "renderer/shader.h"
#include <fstream>
#include <sstream>
#include "pch.h"

Shader::Shader(const char* shaderPath){
    std::cout<<"Made shader!\n";
    if (!construct(shaderPath))
        std::cerr << "ERROR : In shader constructor. Shader creation failed!\n";
}

Shader::~Shader(){
    glDeleteProgram(programID);
}

bool Shader::readShaders(const char* shaderPath, std::string& vs, std::string& fs){
    std::ifstream file;
    file.open(shaderPath);
    if (file.fail()){
        std::cerr << "ERROR: In Shader::readShaders. Unable to open the file -- " << shaderPath << " --\n";
        return false;
    }

    std::stringstream streams[2];
    std::string line;
    int a = 0;
    while (file.good()){
        std::getline(file, line);
        if (line.find("#type") != std::string::npos){
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
    return true;

}

bool Shader::construct(const char* shaderPath){
    std::string vertexShader, fragmentShader;
    if (!readShaders(shaderPath, vertexShader, fragmentShader)){
        std::cerr << "ERROR: In shader::compile. Failed to read shaders from " << shaderPath << '\n';
        return false;
    }

    int vShader = createShader(vertexShader, GL_VERTEX_SHADER);
    glCompileShader(vShader);

    int success = 0;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);

    if(success == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &maxLength);

        char buffer[maxLength];
        glGetShaderInfoLog(vShader, maxLength, &maxLength, &buffer[0]);
        std::cout << "Vertex shader failed to compile : " << buffer << '\n';

        glDeleteShader(vShader); 
        return false;
    }

    int fShader = createShader(fragmentShader, GL_FRAGMENT_SHADER);
    glCompileShader(fShader);

    success = 0;
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &maxLength);

        char buffer[maxLength];
        glGetShaderInfoLog(fShader, maxLength, &maxLength, &buffer[0]);
        std::cout << "Vertex shader failed to compile : " << buffer << '\n';

        glDeleteShader(fShader); 
        glDeleteShader(vShader); 
        return false;
    }

    programID = createProgram(vShader, fShader);

    //check success status
    success = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if (!success){
        char buffer[512];
        glGetProgramInfoLog(programID, 512, NULL, buffer);

        std::cerr << "ERROR: In shader::compile. Failed to link shaders from " << shaderPath << '\n'
        << buffer << '\n';

        glDeleteShader(vShader);
        glDeleteShader(fShader);
        glDeleteProgram(programID);
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

void Shader::setBool(const std::string& name, bool value) const
{         
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string& name, int value) const
{ 
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value); 
}
void Shader::setFloat(const std::string& name, float value) const
{ 
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value); 
} 

void Shader::setVec2(const std::string& name, const glm::vec2& vec) const
{ 
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 2,  (GLfloat*) &vec); 
} 

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const
{ 
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 3,  (GLfloat*) &vec); 
} 

void Shader::setVec4(const std::string& name, const glm::vec4& vec) const
{ 
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 4,  (GLfloat*) &vec); 
} 

void Shader::setmat3(const std::string& name, const glm::mat3x3& mat) const
{ 
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]); 
} 

void Shader::setmat4(const std::string& name, const glm::mat4& mat) const
{ 
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]); 
} 

void Shader::uploadTexture(const std::string& name, int slot) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), slot);
}

void Shader::uploadTextures(const std::string& name, int* slots) const
{
    glUniform1iv(glGetUniformLocation(programID, name.c_str()), 8, slots);
}

void Shader::activate(){
    glUseProgram(programID);
}

void Shader::deActivate(){
    glUseProgram(0);
}