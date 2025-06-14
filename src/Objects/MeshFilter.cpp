#include "Objects/MeshFilter.h"

#include "Objects/MeshFilter.h"
#include <Engine/MeshFactory.h>

MeshFilter::MeshFilter(std::shared_ptr<Mesh> meshPtr, PrimitiveType type)
    : mesh(std::move(meshPtr)), primitiveType(type)
{
    meshName = PrimitiveTypeToString(primitiveType);
}

std::string MeshFilter::GetTypeName() const {
    return "MeshFilter";
}

void MeshFilter::OnInspectorGUI() {
    ImGui::Text("Mesh Filter");

    const char* primitiveLabels[] = { "Cube", "Sphere", "Plane", "Capsule" };
    int current = static_cast<int>(primitiveType);

    if (ImGui::Combo("Primitive Type", &current, primitiveLabels, IM_ARRAYSIZE(primitiveLabels))) {
        primitiveType = static_cast<PrimitiveType>(current);
        meshName = PrimitiveTypeToString(primitiveType);
        mesh = MeshFactory::CreatePrimitive(primitiveType);
    }

    ImGui::Text("Mesh name: %s", meshName.c_str());
}
