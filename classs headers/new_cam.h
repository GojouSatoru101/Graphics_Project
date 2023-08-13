#pragma once
#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    MATHI,
    TALA,
    DEFAULT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 4.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 70.0f;

// default screen values
const int SCR_width = 800;
const int SCR_height = 600;
// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float lastX;
    float lastY;

    //store accumulated scene rotation
    float sceneYaw = 0.0f;
    float scenePitch = 0.0f;

    // last Yaw and Pitch
    float lastYaw;
    float lastPitch;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), int SCR_WIDTH = SCR_width, int SCR_HEIGHT = SCR_height, float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        lastX = SCR_WIDTH / 2.0;
        lastY = SCR_HEIGHT / 2.0;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, int SCR_WIDTH = SCR_width, int SCR_HEIGHT = SCR_height) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        lastX = SCR_WIDTH / 2.0;
        lastY = SCR_HEIGHT / 2.0;
        updateCameraVectors();
    }

    glm::mat4 GetSceneRotationMatrix()
    {
        glm::mat4 sceneRotation = glm::mat4(1.0f);
        sceneRotation = glm::rotate(sceneRotation, glm::radians(scenePitch), glm::vec3(1.0f, 0.0f, 0.0f));
        sceneRotation = glm::rotate(sceneRotation, glm::radians(sceneYaw), glm::vec3(0.0f, 1.0f, 0.0f));
        return sceneRotation;
    }


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        glm::mat4 view=glm::lookAt(Position, Position + Front, Up);
        glm::mat4 sceneRotation = GetSceneRotationMatrix();
        return view * sceneRotation; // Apply scene rotation to the view matrix
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        if (direction == DEFAULT)
        {
            Position = glm::vec3(0.88f, 8.8f, 28.0f);
            WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            Yaw = YAW;
            Pitch = PITCH;
            lastX = 800.0 / 2.0;
            lastY = 600.0 / 2.0;
            Zoom = ZOOM;
            sceneYaw = 0.0f;    // Reset accumulated scene rotation
            scenePitch = 0.0f;  // Reset accumulated scene rotation
            updateCameraVectors();
        }

        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == MATHI)
            Position += Up * velocity;
        if (direction == TALA)
            Position -= Up * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool isMouseDragging,GLboolean constrainPitch = true)
    {
        if (isMouseDragging)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            sceneYaw += xoffset;
            scenePitch += yoffset;
        }
        //// make sure that when pitch is out of bounds, screen doesn't get flipped
        //if (constrainPitch)
        //{
        //    if (Pitch > 89.0f)
        //        Pitch = 89.0f;
        //    if (Pitch < -89.0f)
        //        Pitch = -89.0f;
        //}

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        /*if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;*/
    }



private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif