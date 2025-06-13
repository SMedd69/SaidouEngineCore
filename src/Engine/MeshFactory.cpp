#include "Engine/MeshFactory.h"

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
        case PrimitiveType::Sphere:
            // TODO
        case PrimitiveType::Plane:
            // TODO
        case PrimitiveType::Capsule:
            // TODO
        default:
            return nullptr;
    }
}
