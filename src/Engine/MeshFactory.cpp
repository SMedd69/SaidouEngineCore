#include "Engine/MeshFactory.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::shared_ptr<Mesh> MeshFactory::CreatePrimitive(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Cube: {
            std::vector<float> vertices = {
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                -0.5f, -0.5f,  0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f
            };

            std::vector<unsigned int> indices = {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4,
                0, 4, 7, 7, 3, 0,
                1, 5, 6, 6, 2, 1,
                0, 1, 5, 5, 4, 0,
                3, 2, 6, 6, 7, 3
            };

            return std::make_shared<Mesh>(vertices, indices);
        }

        case PrimitiveType::Plane: {
            std::vector<float> vertices = {
                -0.5f, 0.0f, -0.5f,
                 0.5f, 0.0f, -0.5f,
                 0.5f, 0.0f,  0.5f,
                -0.5f, 0.0f,  0.5f
            };

            std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0
            };

            return std::make_shared<Mesh>(vertices, indices);
        }

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

                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
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

        case PrimitiveType::Capsule: {
            // Capsule = 2 hemispheres + cylinder
            // Pour simplifier : approximons une capsule verticalement

            const unsigned int slices = 16;
            const unsigned int stacks = 8;
            const float radius = 0.25f;
            const float height = 1.0f;

            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            // Generate hemisphere (top)
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
                }
            }

            // Generate cylinder body
            for (unsigned int y = 0; y <= 1; ++y) {
                for (unsigned int x = 0; x <= slices; ++x) {
                    float xSegment = (float)x / slices;
                    float theta = xSegment * 2.0f * M_PI;

                    float xPos = radius * std::cos(theta);
                    float yPos = height * (0.5f - y);
                    float zPos = radius * std::sin(theta);

                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
                }
            }

            // Generate hemisphere (bottom)
            for (unsigned int y = 0; y <= stacks; ++y) {
                for (unsigned int x = 0; x <= slices; ++x) {
                    float xSegment = (float)x / slices;
                    float ySegment = (float)y / stacks;
                    float theta = xSegment * 2.0f * M_PI;
                    float phi = ySegment * M_PI / 2.0f;

                    float xPos = radius * std::cos(theta) * std::sin(phi);
                    float yPos = -radius * std::cos(phi) - height / 2.0f;
                    float zPos = radius * std::sin(theta) * std::sin(phi);

                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
                }
            }

            // Pour les indices : ici tu peux créer les triangles comme dans le cas de la sphère
            // ou ajouter un TODO si tu veux améliorer les performances ou le style

            return std::make_shared<Mesh>(vertices, indices);
        }

        default:
            return nullptr;
    }
}
