#include <UI/GameWindow.h>
#include <imgui.h>
#include <Objects/CameraComponent.h>
#include <Core/Scene.h>
#include <Objects/TransformComponent.h>
#include <Engine/Renderer.h>

void GameWindow::Render()
{
    ImGui::Begin("Game");

    // --- Choix du format d'affichage ---
    static int formatIndex = 1;
    static const char* formats[] = { "Libre", "1920x1080", "1280x720", "800x600", "16:9", "4:3", "1:1" };
    static ImVec2 renderSize = ImVec2(1920, 1080);
    static float aspectRatios[] = { 0.0f, 1920.0f/1080.0f, 1280.0f/720.0f, 800.0f/600.0f, 16.0f/9.0f, 4.0f/3.0f, 1.0f };

    ImGui::Text("Format : ");
    ImGui::SameLine();
    ImGui::Combo("##Format", &formatIndex, formats, IM_ARRAYSIZE(formats));

    // --- Détermine la taille de rendu ---
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 imageSize = avail;
    float targetAspect = 1.0f;

    if (formatIndex == 1) { renderSize = ImVec2(1920, 1080); targetAspect = 1920.0f/1080.0f; }
    else if (formatIndex == 2) { renderSize = ImVec2(1280, 720); targetAspect = 1280.0f/720.0f; }
    else if (formatIndex == 3) { renderSize = ImVec2(800, 600); targetAspect = 800.0f/600.0f; }
    else if (formatIndex > 3) { // Ratio fixe
        targetAspect = aspectRatios[formatIndex];
        renderSize = ImVec2(avail.x, avail.x / targetAspect);
        if (avail.x / avail.y > targetAspect) {
            imageSize.x = avail.y * targetAspect;
            imageSize.y = avail.y;
        } else {
            imageSize.x = avail.x;
            imageSize.y = avail.x / targetAspect;
        }
        renderSize = ImVec2(imageSize.x, imageSize.y);
    } else { // Libre
        renderSize = avail;
        imageSize = avail;
        targetAspect = avail.x / avail.y;
    }

    // --- Rendu caméra ---
    auto scene = m_scene;
    std::shared_ptr<GameObject> cameraObj = nullptr;
    CameraComponent* camera = nullptr;
    TransformComponent* camTransform = nullptr;

    for (auto& obj : scene->gameObjects) {
        camera = obj->GetComponent<CameraComponent>();
        camTransform = obj->GetComponent<TransformComponent>();
        if (camera && camTransform) {
            cameraObj = obj;
            break;
        }
    }

    if (camera && camTransform) {
        float aspect = targetAspect;
        glm::mat4 view = glm::inverse(camTransform->GetWorldTransformMatrix());
        glm::mat4 proj = camera->GetProjectionMatrix(aspect);

        unsigned int gameTexture = Renderer::RenderSceneToTexture(scene, view, proj, (int)renderSize.x, (int)renderSize.y);

        // Centre l'image dans la fenêtre Game (letterbox/pillarbox)
        ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
        ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 contentMin = ImVec2(windowPos.x + regionMin.x, windowPos.y + regionMin.y);

        ImVec2 centerPos = ImVec2(
            (avail.x - imageSize.x) * 0.5f,
            (avail.y - imageSize.y) * 0.5f
        );

        ImGui::SetCursorPos(ImVec2(regionMin.x + centerPos.x, regionMin.y + centerPos.y));

        ImGui::Image((ImTextureID)(uintptr_t)gameTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));

        // Affiche la résolution courante
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
        ImGui::Text("Resolution : %dx%d", (int)renderSize.x, (int)renderSize.y);
    } else {
        ImGui::Text("Aucune caméra dans la scène !");
    }

    ImGui::End();
}