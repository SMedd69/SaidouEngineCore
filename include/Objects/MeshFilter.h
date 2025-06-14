#pragma once
#include <Objects/Component.h>
#include <imgui.h>
#include <string>
#include <memory>
#include <Engine/Mesh.h>
#include <Engine/MeshFactory.h>

class MeshFilter : public Component {
public:
    std::shared_ptr<Mesh> mesh;
    PrimitiveType primitiveType = PrimitiveType::Cube;
    std::string meshName = "Cube";

    MeshFilter() = default;
    explicit MeshFilter(std::shared_ptr<Mesh> meshPtr, PrimitiveType type = PrimitiveType::Cube);

    std::string GetTypeName() const override;
    void OnInspectorGUI() override;
};
