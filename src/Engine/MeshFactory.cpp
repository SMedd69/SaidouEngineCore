#include "Engine/MeshFactory.h"
#include <cmath>
#include <glm/glm.hpp>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::shared_ptr<Mesh> MeshFactory::CreatePrimitive(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Cube: {
            // 6 faces * 4 sommets = 24 sommets (chaque face a sa normale)
            std::vector<float> vertices = {
                // Face avant (z+)
                -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
                 0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
                 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
                // Face arrière (z-)
                -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
                 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
                 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
                -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
                // Face gauche (x-)
                -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
                // Face droite (x+)
                 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
                 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
                // Face bas (y-)
                -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
                 0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
                 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,
                -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,
                // Face haut (y+)
                -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
            };

            std::vector<unsigned int> indices = {
                0, 1, 2, 2, 3, 0,        // avant
                4, 5, 6, 6, 7, 4,        // arrière
                8, 9,10,10,11, 8,        // gauche
               12,13,14,14,15,12,        // droite
               16,17,18,18,19,16,        // bas
               20,21,22,22,23,20         // haut
            };

            return std::make_shared<Mesh>(vertices, indices);
        }

        case PrimitiveType::Plane: {
            std::vector<float> vertices = {
                //  pos                normal
                -0.5f, 0.0f, -0.5f,    0.0f, 1.0f, 0.0f,
                 0.5f, 0.0f, -0.5f,    0.0f, 1.0f, 0.0f,
                 0.5f, 0.0f,  0.5f,    0.0f, 1.0f, 0.0f,
                -0.5f, 0.0f,  0.5f,    0.0f, 1.0f, 0.0f,
            };

            std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0
            };

            return std::make_shared<Mesh>(vertices, indices);
        }

        // Pour Sphere et Capsule, il faut aussi ajouter les normales à chaque sommet.
        // Pour la sphère, la normale à chaque sommet est simplement la position normalisée.

        case PrimitiveType::Sphere: {
            const unsigned int X_SEGMENTS = 16;
            const unsigned int Y_SEGMENTS = 16;
            const float radius = 0.5f;

            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                    float xSegment = (float)x / (float)X_SEGMENTS;
                    float ySegment = (float)y / (float)Y_SEGMENTS;
                    float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI) * radius;
                    float yPos = std::cos(ySegment * M_PI) * radius;
                    float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI) * radius;

                    // Position
                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
                    // Normale (position normalisée)
                    float length = std::sqrt(xPos*xPos + yPos*yPos + zPos*zPos);
                    vertices.push_back(xPos / length);
                    vertices.push_back(yPos / length);
                    vertices.push_back(zPos / length);
                }
            }

            for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
                for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
                    unsigned int i0 = y * (X_SEGMENTS + 1) + x;
                    unsigned int i1 = i0 + X_SEGMENTS + 1;

                    indices.push_back(i0);
                    indices.push_back(i1);
                    indices.push_back(i0 + 1);

                    indices.push_back(i0 + 1);
                    indices.push_back(i1);
                    indices.push_back(i1 + 1);
                }
            }

            return std::make_shared<Mesh>(vertices, indices);
        }

        // Capsule : même principe, il faut ajouter les normales à chaque sommet.
        // Pour simplifier, on peut utiliser la position normalisée pour chaque sommet.

        case PrimitiveType::Capsule: {
            const unsigned int slices = 16;
            const unsigned int stacks = 8;
            const float radius = 0.25f;
            const float height = 1.0f;

            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            // Hemisphere top
            for (unsigned int y = 0; y <= stacks; ++y) {
                for (unsigned int x = 0; x <= slices; ++x) {
                    float xSegment = (float)x / slices;
                    float ySegment = (float)y / stacks;
                    float theta = xSegment * 2.0f * M_PI;
                    float phi = ySegment * M_PI / 2.0f;

                    float xPos = radius * std::cos(theta) * std::sin(phi);
                    float yPos = radius * std::cos(phi) + height / 2.0f;
                    float zPos = radius * std::sin(theta) * std::sin(phi);

                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);

                    // Normale (par rapport au centre de la sphère du haut)
                    glm::vec3 normal = glm::normalize(glm::vec3(xPos, yPos - height / 2.0f, zPos));
                    vertices.push_back(normal.x);
                    vertices.push_back(normal.y);
                    vertices.push_back(normal.z);
                }
            }
            // ... (idem pour le corps et l'hemisphère du bas, à adapter selon ta logique)

            // Indices à adapter selon la structure

            return std::make_shared<Mesh>(vertices, indices);
        }

        default:
            return nullptr;
    }
}