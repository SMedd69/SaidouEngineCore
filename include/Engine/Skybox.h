#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>

class Shader;
class TextureCube;

class Skybox {
public:
    Skybox(const std::string& folderPath, bool isCross); // Charge les 6 faces à partir d'un dossier
    void Draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& sunDir);

private:
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<TextureCube> m_cubemap;
    unsigned int m_vao = 0, m_vbo = 0;
};