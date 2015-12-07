#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#ifdef CLANG_COMPLETE_ONLY
    #define GL_GLEXT_PROTOTYPES
#ifdef __LINUXOS__
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif
#ifdef __APPLEOS__
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
#endif
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    W_ADD,
    W_SUB,
    YZ_ROT,
};

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat ROTATION_SPEED = glm::radians(3.0f);
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;

class Camera
{
    public:
        // Camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        GLfloat w;
        GLfloat w_unit;
        GLfloat theta_yz;
        // Euler Angles
        GLfloat Yaw;
        GLfloat Pitch;
        // Camera options
        GLfloat MovementSpeed;
        GLfloat RotationSpeed;
        GLfloat MouseSensitivity;
        GLfloat Zoom;

        // Constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, 
               GLfloat pitch = PITCH);

        // Constructor with scalar values
        Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, 
               GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);

        // Returns the view matrix calculated using Euler Angles and the 
        // LookAt Matrix
        glm::mat4 GetViewMatrix();
        
        glm::mat4 GetModel4D();

        // Processes input received from any keyboard-like input system. 
        // Accepts input parameter in the form of camera defined ENUM 
        // (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);

        // Processes input received from a mouse input system. Expects the 
        // offset value in both the x and y direction.
        void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, 
                                  GLboolean constrainPitch = true);

        // Processes input received from a mouse scroll-wheel event. 
        // Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(GLfloat yoffset);

    private:
        // Calculates the front vector from the Camera's (updated) Euler angles
        void updateCameraVectors();

};
#endif
