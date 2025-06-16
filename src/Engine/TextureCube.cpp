#include <Engine/TextureCube.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vector>
#include <cstring>

TextureCube::TextureCube(const std::string& folderPath) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    std::vector<std::string> faces = {
        "right.tga",
        "left.tga",
        "top.tga",
        "bottom.tga",
        "back.tga",
        "front.tga"
    };

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        std::string path = folderPath + "/" + faces[i];
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

TextureCube::TextureCube(const std::string& crossImagePath, bool isCross) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(crossImagePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) return;

    int faceSize = width / 4; // ou height / 3
    if (height / 3 != faceSize) {
        stbi_image_free(data);
        return; // format non supporté
    }

    // Buffers pour chaque face
    std::vector<unsigned char*> faces(6, nullptr);
    for (int i = 0; i < 6; ++i)
        faces[i] = new unsigned char[faceSize * faceSize * nrChannels];

    // Helper pour copier une face
    auto copy_face = [&](int dst, int col, int row) {
        for (int y = 0; y < faceSize; ++y) {
            std::memcpy(
                faces[dst] + y * faceSize * nrChannels,
                data + ((row * faceSize + y) * width + col * faceSize) * nrChannels,
                faceSize * nrChannels
            );
        }
    };

    // OpenGL order: right, left, top, bottom, front, back
    copy_face(0, 2, 1); // right
    copy_face(1, 0, 1); // left
    copy_face(2, 1, 0); // top
    copy_face(3, 1, 2); // bottom
    copy_face(4, 1, 1); // front
    copy_face(5, 3, 1); // back

    // Upload chaque face
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            nrChannels == 4 ? GL_RGBA : GL_RGB,
            faceSize, faceSize, 0,
            nrChannels == 4 ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE, faces[i]
        );
        delete[] faces[i];
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

TextureCube::~TextureCube() {
    glDeleteTextures(1, &m_id);
}