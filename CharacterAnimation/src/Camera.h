#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    enum class Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float Acceleration;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 100.0f, 200.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = -25.0f)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          MovementSpeed(50.0f),
          Acceleration(1.0f),
          MouseSensitivity(0.3f),
          Zoom(70.0f),
          Yaw(yaw),
          Pitch(pitch)
    {
        Position = position;
        WorldUp = up;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system
    void ProcessKeyboard(Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * Acceleration * deltaTime;
        if (direction == Movement::FORWARD)
            Position += Front * velocity;
        if (direction == Movement::BACKWARD)
            Position -= Front * velocity;
        if (direction == Movement::LEFT)
            Position -= Right * velocity;
        if (direction == Movement::RIGHT)
            Position += Right * velocity;
        if (direction == Movement::UP)
            Position += WorldUp * velocity;
        if (direction == Movement::DOWN)
            Position -= WorldUp * velocity;
    }

    // Processes input received from a mouse input system
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event
    void ProcessMouseScroll(float yoffset)
    {
        // Control acceleration instead of FOV
        Acceleration += yoffset * 0.5f;
        if (Acceleration < 0.5f)
            Acceleration = 0.5f;
        if (Acceleration > 5.0f)
            Acceleration = 5.0f;
    }

    // Set movement speed
    void SetMovementSpeed(float speed)
    {
        MovementSpeed = speed;
    }

    // Get current movement speed
    float GetMovementSpeed() const
    {
        return MovementSpeed;
    }

    // Get current acceleration multiplier
    float GetAcceleration() const
    {
        return Acceleration;
    }

    // Get zoom (FOV)
    float GetZoom() const
    {
        return Zoom;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
