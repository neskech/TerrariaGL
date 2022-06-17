#pragma once
#include "pch.h"
#include <string>

class Shader{
    public:
        Shader(const char* shaderPath);
        Shader(const Shader& other) = delete;
        Shader(const Shader&& other) = delete;
        ~Shader();

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const glm::vec2& vec) const;
        void setVec3(const std::string& name, const glm::vec3& vec) const;
        void setVec4(const std::string& name, const glm::vec4& vec) const;
        void setmat3(const std::string& name, const glm::mat3x3& vec) const;
        void setmat4(const std::string& name, const glm::mat4x4& vec) const;
        void uploadTexture(const std::string& name, int slot) const;
        void uploadTextures(const std::string& name, int* slots) const;

        inline void activate(){ glUseProgram(programID); }
        inline void deActivate(){ glUseProgram(0); }
        inline int getID(){ return programID; }
    private:
        bool readShaders(const char* shaderPath, std::string& vs, std::string& fs);
        int createShader(std::string& shaderSrc, int shaderType);
        int createProgram(int vShader, int fShader);
        bool construct(const char* shaderPath);

        int programID;
};