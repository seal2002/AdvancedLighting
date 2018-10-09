#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#define CAMERA_OUTPUT 0
// Defines several possible options for camera movement

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    ROTATE_UP,
    ROTATE_DOWN,
    ZOOM_IN,
    ZOOM_OUT
};

// Default camera values
const float YAW	= -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.2f;
const float ZOOM  = 45.0f;

class Camera
{
public:
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw;
    float pitch;
    float zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float zoom = ZOOM)
    {
        this->cameraPos = position;
        this->cameraUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->zoom = zoom;
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    void ProcessKeyboard(Camera_Movement direction)
    {
        if (direction == Camera_Movement::FORWARD)
            cameraPos += SPEED * cameraFront;
        if (direction == Camera_Movement::BACKWARD)
            cameraPos -= SPEED * cameraFront;
        if (direction == Camera_Movement::LEFT)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * SPEED;
        if (direction == Camera_Movement::RIGHT)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * SPEED;
        if (direction == Camera_Movement::ROTATE_LEFT)
            yaw -= 1.0f;
        if (direction == Camera_Movement::ROTATE_RIGHT)
            yaw += 1.0f;
        if (direction == Camera_Movement::ROTATE_UP)
            pitch += 1.0f;
        if (direction == Camera_Movement::ROTATE_DOWN)
            pitch -= 1.0f;
        if (direction == Camera_Movement::ZOOM_IN)
            zoom -= 0.01f;
        if (direction == Camera_Movement::ZOOM_OUT)
            zoom += 0.01f;
        updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        cameraFront = glm::normalize(front);
    #if CAMERA_OUTPUT
        std::cout << "yaw:" << yaw << " pitch:" << pitch << " zoom:" << zoom << " x:" << cameraFront.x << " y:" << cameraFront.y << " z:" << cameraFront.z << std::endl;
    #endif // 0
    }
};
