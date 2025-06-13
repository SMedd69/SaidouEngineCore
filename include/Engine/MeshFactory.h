#pragma once
#include <memory>
#include "Engine/Mesh.h"

enum class PrimitiveType {
    Cube,
    Sphere,
    Plane,
    Capsule
};

class MeshFactory {
public:
    static std::shared_ptr<Mesh> CreatePrimitive(PrimitiveType type);
};
