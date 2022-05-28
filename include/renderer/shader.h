#pragma once
#include <string>
#include "pch.h"

class Shader{
    public:
        Shader(const char* shaderPath);
        Shader(const Shader& other) = delete;
        Shader(const Shader&& other) = delete;
        ~Shader();

        void use();
        void deActivate();

        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, glm::vec2 vec) const;
        void setVec3(const std::string &name, glm::vec3 vec) const;
        void setVec4(const std::string &name, glm::vec4 vec) const;
        void setmat3(const std::string &name, glm::mat3x3 vec) const;
        void setmat4(const std::string &name, glm::mat4x4 vec) const;

        inline int getID(){ return programID; }
    private:
        bool readShaders(const char* shaderPath, std::string& vs, std::string& fs);
        int createShader(std::string& shaderSrc, int shaderType);
        int createProgram(int vShader, int fShader);
        bool construct(const char* shaderPath);

        int programID;
};