#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x3.hpp>
class CameraOrbit {
public:
    glm::vec3 target = glm::vec3(0.0f);
    float distance = 10.0f;
    float pitch = glm::radians(30.0f); // angle vertical
    float yaw = glm::radians(45.0f);   // angle horizontal
    float fov = 45.0f;

    CameraOrbit() = default;

    glm::mat4 GetViewMatrix() const {
        glm::vec3 pos = target + glm::vec3(
            distance * cos(pitch) * sin(yaw),
            distance * sin(pitch),
            distance * cos(pitch) * cos(yaw)
        );
        return glm::lookAt(pos, target, glm::vec3(0,1,0));
    }

    void ProcessMouseMovement(float dx, float dy, bool orbiting, bool panning) {
        if (orbiting) {
            float sensitivity = 0.005f;
            yaw   -= dx * sensitivity;
            pitch -= dy * sensitivity;

            // Clamp pitch pour éviter que la caméra passe sous la scène
            const float limit = glm::radians(89.0f);
            if (pitch > limit) pitch = limit;
            if (pitch < -limit) pitch = -limit;
        }
        else if (panning) {
            // Calcul du déplacement du target en fonction de la caméra
            float panSpeed = 0.01f * distance;
            glm::vec3 right = glm::normalize(glm::cross(GetDirection(), glm::vec3(0,1,0)));
            glm::vec3 up = glm::normalize(glm::cross(right, GetDirection()));
            target += -right * dx * panSpeed + up * dy * panSpeed;
        }
    }

    void ProcessMouseScroll(float delta) {
        float zoomSpeed = 1.0f;
        distance -= delta * zoomSpeed;
        if (distance < 1.0f) distance = 1.0f; // limite zoom trop près
        if (distance > 100.0f) distance = 100.0f; // limite zoom trop loin
    }

private:
    glm::vec3 GetDirection() const {
        return glm::normalize(target - GetPosition());
    }
    glm::vec3 GetPosition() const {
        return target + glm::vec3(
            distance * cos(pitch) * sin(yaw),
            distance * sin(pitch),
            distance * cos(pitch) * cos(yaw)
        );
    }
};
