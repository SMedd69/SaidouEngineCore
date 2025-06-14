#pragma once
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Setters uniform
    void SetMat4(const std::string& name, const glm::mat4& value);
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);

private:
    unsigned int m_id;
    std::unordered_map<std::string, int> m_uniformLocationCache;

    std::string LoadShaderSource(const std::string& path);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    void CheckCompileErrors(unsigned int shader, std::string type);
    int GetUniformLocation(const std::string& name);
};
