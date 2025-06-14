#pragma once
#include <memory>
#include <string>
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

inline std::string PrimitiveTypeToString(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Cube: return "Cube";
        case PrimitiveType::Sphere: return "Sphere";
        case PrimitiveType::Plane: return "Plane";
        case PrimitiveType::Capsule: return "Capsule";
        default: return "Unknown";
    }
}

inline PrimitiveType StringToPrimitiveType(const std::string& name) {
    if (name == "Cube") return PrimitiveType::Cube;
    if (name == "Sphere") return PrimitiveType::Sphere;
    if (name == "Plane") return PrimitiveType::Plane;
    if (name == "Capsule") return PrimitiveType::Capsule;
    return PrimitiveType::Cube; // fallback
}
