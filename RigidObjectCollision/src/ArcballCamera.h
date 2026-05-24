#ifndef __ARCBALLCAMERA_H__
#define __ARCBALLCAMERA_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class ArcballCamera {
private:
    // Arcball data (for legacy compatibility)
    glm::quat rotation;
    glm::vec3 lastPos;
    bool dragging = false;

    // Free-fly camera data
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 cameraUp;

    float pitch = 0.0f;  // rotation around right axis (X)
    float yaw = 0.0f;    // rotation around up axis (Y)
    float cameraSpeed = 50.0f;
    float acceleration = 1.0f;

    bool rightMouseDown = false;

public:
    glm::vec3 eye;
    glm::vec3 target;
    glm::vec3 up;

    float distance;
    float fov, aspect;
    float near;
    float far;

    glm::mat4 matView;
    glm::mat4 matProj;


public:
    ArcballCamera(glm::vec3 e, glm::vec3 tgt, float distance, float fov, float aspect, float n, float f)
        : eye(e), target(tgt), distance(distance), fov(fov), aspect(aspect), near(n), far(f)
    {
        rotation = glm::quat(1,0,0,0); 
        matProj = glm::perspective(glm::radians(fov), aspect, near, far);

        // Initialize free-fly directions
        forward = glm::normalize(target - eye);
        right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        cameraUp = glm::normalize(glm::cross(right, forward));
        up = cameraUp;

        UpdateView();
    }

    void MouseDown(double x, double y) {
        dragging = true;
        lastPos = ScreenToArcball(x, y);
    }

    void MouseUp() {
        dragging = false;
    }

    void MouseMove(double x, double y) {
        if (!dragging) return;

        glm::vec3 currPos = ScreenToArcball(x, y);

        // Compute quaternion between last and current
        glm::vec3 axis = glm::cross(lastPos, currPos);
        // angle between v0 and v1
        float dot = glm::dot(lastPos, currPos);

        float angle = -acos(glm::min(1.0f, glm::max(-1.0f, dot)));

        glm::quat dq = glm::angleAxis(angle, glm::normalize(axis));
        rotation = dq * rotation;

        lastPos = currPos;

        UpdateView();
    }

    // Right mouse button pan and tilt (UE5 style)
    void RightMouseDown(double x, double y) {
        rightMouseDown = true;
        lastX = x;
        lastY = y;
    }

    void RightMouseUp() {
        rightMouseDown = false;
    }

    void RightMouseMove(double x, double y) {
        if (!rightMouseDown) return;

        double deltaX = x - lastX;
        double deltaY = y - lastY;

        // Yaw (left-right look)
        yaw += (float)deltaX * 0.003f;

        // Pitch (up-down look)
        pitch -= (float)deltaY * 0.003f;

        // Clamp pitch to avoid flipping
        pitch = glm::clamp(pitch, -glm::pi<float>() * 0.49f, glm::pi<float>() * 0.49f);

        UpdateFreeFlyView();

        lastX = x;
        lastY = y;
    }

    // Scroll wheel controls acceleration
    void Zoom(float delta) {
        acceleration *= (1.0f + delta * 0.2f);
        acceleration = glm::clamp(acceleration, 0.1f, 5.0f);
    }

    // WASD movement
    void MoveForward(float dt) {
        eye += forward * cameraSpeed * acceleration * dt;
        target = eye + forward * distance;
        UpdateView();
    }

    void MoveBackward(float dt) {
        eye -= forward * cameraSpeed * acceleration * dt;
        target = eye + forward * distance;
        UpdateView();
    }

    void MoveRight(float dt) {
        eye += right * cameraSpeed * acceleration * dt;
        target = eye + forward * distance;
        UpdateView();
    }

    void MoveLeft(float dt) {
        eye -= right * cameraSpeed * acceleration * dt;
        target = eye + forward * distance;
        UpdateView();
    }

    void MoveUp(float dt) {
        eye += cameraUp * cameraSpeed * acceleration * dt;
        target = eye + forward * distance;
        UpdateView();
    }

    void MoveDown(float dt) {
        eye -= cameraUp * cameraSpeed * acceleration * dt;
        target = eye + forward * distance;
        UpdateView();
    }

    // Pan with middle mouse (legacy)
    void Pan(float dx, float dy) {
        glm::vec3 panRight = -right * dx * distance * 0.001f;
        glm::vec3 panUp = cameraUp * dy * distance * 0.001f;

        eye += panRight + panUp;
        target += panRight + panUp;
        UpdateView();
    }

    void setEye(glm::vec3 e) { eye = e; }
    void setTarget(glm::vec3 t) {target = t; }

    void reset(glm::vec3 e, glm::vec3 t, glm::vec3 u = glm::vec3(0, 1, 0)) {
        setEye(e);
        setTarget(t);
        up = u;

        rotation = glm::quat(1,0,0,0); 
        pitch = 0.0f;
        yaw = 0.0f;
        acceleration = 1.0f;

        forward = glm::normalize(target - eye);
        right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        cameraUp = glm::normalize(glm::cross(right, forward));

        matView = glm::lookAt(eye, target, glm::vec3(0,1,0));
    }

    const glm::mat4& GetView()     const { return matView; }
    const glm::mat4& GetProjection() const { return matProj; }

    void SetViewportSize(float width, float height) {
        aspect = width / height;
        matProj = glm::perspective(glm::radians(fov), aspect, 0.01f, 10000.0f);
    }

private:
    double lastX, lastY;

    glm::vec3 ScreenToArcball(double x, double y) {
        // Convert to [-1,1] range
        float nx = (2.0f * float(x)) - 1.0f;
        float ny = 1.0f - (2.0f * float(y));

        glm::vec2 p(nx, ny);
        float len2 = glm::dot(p, p);

        // try to set a z
        if (len2 <= 1.0f) {
            float z = sqrtf(1.0f - len2);
            return glm::vec3(p, z);
        } else {
            glm::vec2 norm = glm::normalize(p);
            return glm::vec3(norm, 0.0f);
        }
    }

    void UpdateView() {
        matView = glm::lookAt(eye, target, cameraUp);
    }

    void UpdateFreeFlyView() {
        // Calculate forward direction from pitch and yaw
        forward.x = sin(yaw) * cos(pitch);
        forward.y = sin(pitch);
        forward.z = -cos(yaw) * cos(pitch);
        forward = glm::normalize(forward);

        // Recalculate right and up vectors
        right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        cameraUp = glm::normalize(glm::cross(right, forward));

        // Update target based on forward direction
        target = eye + forward * distance;

        matView = glm::lookAt(eye, target, cameraUp);
    }
};

#endif