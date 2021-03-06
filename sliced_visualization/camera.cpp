#include "camera.h"
#include <iostream>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) 
               : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
                 w(0.0f),
                 w_unit(1.0f),
                 theta(0.0f),
                 MovementSpeed(SPEED), 
                 RotationSpeed(ROTATION_SPEED),
                 MouseSensitivity(SENSITIVTY),
                 Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, 
               GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) 
               : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), 
                 MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Returns the view matrix calculated using Euler angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetRotMatrix4D()
{
    glm::mat4 model4D(1.0f);
    GLfloat cs0 = cos(-theta), sn0 = sin(-theta);
    model4D[0][0] = cs0;
    model4D[3][0] = -sn0;
    model4D[0][3] = sn0;
    model4D[3][3] = cs0;
    return model4D;
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = MovementSpeed * deltaTime;
    glm::vec4 pos = glm::vec4(Front, w);
    glm::mat4 rot = GetRotMatrix4D();
    glm::vec4 radj = rot * glm::vec4(Front, w);
    glm::vec4 fadj = rot * glm::vec4(Front, w);
    switch (direction) {
        case FORWARD:
            Position += Front * velocity;
            break;
        case BACKWARD:
            Position -= Front * velocity;
            break;
        case LEFT:
            Position -= Right * velocity;
            break;
        case RIGHT:
            Position += Right * velocity;
            break;
        case W_ADD:
            w += w_unit * velocity;
            break;
        case W_SUB:
            w -= w_unit * velocity;
            break;
        case ROT_INC:
            theta += ROTATION_SPEED * deltaTime;
            while (theta >= glm::radians(360.0f)) 
                theta -= glm::radians(360.0f);
            break;
        case ROT_DEC:
            theta -= ROTATION_SPEED * deltaTime;
            while (theta <= glm::radians(-360.0f)) 
                theta += glm::radians(360.0f);
            break;
        default:
            break;
    }
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}
