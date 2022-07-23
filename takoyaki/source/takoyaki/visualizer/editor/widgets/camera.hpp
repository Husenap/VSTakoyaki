#pragma once

#include <array>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "takoyaki/visualizer/util/input.hpp"
#include "widget.hpp"

namespace ht {

class Camera : public Widget {
public:
    Camera();

    void Reset();
    void Update(float deltaTime);

    void ProcessKeyInput(const KeyInput& input);
    void ProcessMouseMovement(const glm::vec2& delta);

    const glm::vec3& GetPosition() const { return mPosition; }
    const glm::vec3& GetTarget() const { return mTarget; }

    void SetActive(bool active) { mIsActive = active; }

private:
    void UpdateMovement(float deltaTime);
    void UpdateCameraVectors();

    glm::vec3 mPosition;
    glm::vec3 mForward;
    glm::vec3 mRight;
    glm::vec3 mUp;
    glm::vec3 mWorldUp;
    glm::vec3 mTarget;
    float     mYaw;
    float     mPitch;
    float     mSpeed;
    float     mSensitivity;
    float     mZoom;

    bool                            mIsActive;
    std::array<bool, GLFW_KEY_LAST> mKeys;
    int                             mKeyMods;
};

}  // namespace ht