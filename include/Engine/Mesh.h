#pragma once
#include <vector>
#include <memory>
#include <glad/gl.h>

class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Draw();

private:
    unsigned int m_VAO, m_VBO, m_EBO;
    size_t m_indexCount;
};
