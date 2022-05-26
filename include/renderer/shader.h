#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader{
    public:
        Shader(const char* path);
        Shader(const Shader& other) = delete;
        Shader(const Shader&& other) = delete;
        ~Shader();

        bool compile();
        void activate();
        void deActivate();

        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, glm::vec2 vec) const;
        void setVec3(const std::string &name, glm::vec3 vec) const;
        void setVec4(const std::string &name, glm::vec4 vec) const;

        inline int getID(){ return programID; }
    private:
        bool readShaders(std::string& vs, std::string& fs);

        int programID;
        const char* shaderPath;
};