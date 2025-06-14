#pragma once
#include <vector>
#include <memory>
#include <glad/gl.h>

class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Bind() const {
        glBindVertexArray(m_VAO);
    }

    size_t GetIndexCount() const {
        return m_indexCount;
    }

    unsigned int GetVBO() const;
    unsigned int GetVAO() const;

    void Draw() const;

private:
    unsigned int m_VAO, m_VBO, m_EBO;
    size_t m_indexCount;
};
