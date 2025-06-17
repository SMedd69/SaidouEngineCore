#include <Objects/DirectionnalLightComponent.h>
#include <imgui.h>

std::string DirectionalLightComponent::GetTypeName() const {
    return "DirectionalLight";
}

void DirectionalLightComponent::OnInspectorGUI() {
    ImGui::Separator();
    ImGui::Text("Directional Light");

    ImGui::DragFloat3("Direction", &direction.x, 0.01f, -360.0f, 360.0f);
    ImGui::ColorEdit3("Color", &color.x);
    ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 10.0f);
}
