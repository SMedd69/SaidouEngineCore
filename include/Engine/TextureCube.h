#pragma once
#include <string>
#include <glad/gl.h>

class TextureCube {
public:
    TextureCube(const std::string& folderPath);
    TextureCube(const std::string& crossImagePath, bool isCross);
    ~TextureCube();
    GLuint GetID() const { return m_id; }
private:
    GLuint m_id = 0;
};