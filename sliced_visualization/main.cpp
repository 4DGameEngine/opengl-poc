#include <iostream>
#include <vector>

#include <cassert>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "shader.h"
#include "camera.h"
#include "utils4D.h"
#include "object4D.h"

// A little bit of hacking here to get my autocomplete to work
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

#include <GLFW/glfw3.h>
#include <SOIL2.h>

using std::vector;
using std::cout;
using std::endl;
using glm::vec4;
using glm::vec3;
using glm::mat4;
using glm::to_string;
using Point_3 = CGAL::Exact_predicates_inexact_constructions_kernel::Point_3;

// Consts
const GLuint WIDTH = 1200, HEIGHT = 900;

// Function prototypes
void do_movement(GLfloat deltaTime);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Camera
vec3 INIT_POS(-2.4f, 3.2f, 5.0f);
Camera camera(INIT_POS);
bool keys[1024];
GLfloat lastX = WIDTH/2, lastY = HEIGHT/2;
bool firstMouse = true;

// Options
GLfloat theta = 0.0f;

int main()
{
    cout << "Starting GLFW context, OpenGL 3.3" << endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // OS X requests 3.3 differently.
#ifdef __APPLEOS__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", 
                                          nullptr, nullptr);    
    if (window == nullptr)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // More hacking for my autocomplete.
#ifndef CLANG_COMPLETE_ONLY
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialize GLEW" << endl;
        return -1;
    }    
#endif

    // Define the viewport dimensions
    // Corrected from the tutorial, should request framebuffer size instead.
    // Technically should also write a callback when the window is resized
    // with framebuffer_size_callback. See GLFW docs. This will do for now.
    int fbwidth, fbheight;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    // Options
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // ---------- BEGIN OPENGL ----------- //
    
    // Hax
    #include "data.cpp"

    // Pack data into vector
    vector<vec4> hyperfrustrum;
    utils4D::packVector4D(vertices_hyperfrustrum, 
                          sizeof(vertices_hyperfrustrum)/(4 * sizeof(GL_FLOAT)),
                          hyperfrustrum);
    
    vector<vec4> hypertetrahedron;
    utils4D::packVector4D(vertices_hypertetrahedron, 
                          sizeof(vertices_hypertetrahedron)/(4 * sizeof(GL_FLOAT)),
                          hypertetrahedron);

    // Shader creation
    Shader frustrumShader;
    frustrumShader.addVert("light_vert.glsl");
    frustrumShader.addFrag("light_frag.glsl");
    frustrumShader.link();

    Shader tetrahedronShader;
    tetrahedronShader.addVert("light_vert.glsl");
    tetrahedronShader.addFrag("light_frag.glsl");
    tetrahedronShader.link();

    Shader cubeShader;
    cubeShader.addVert("light_vert.glsl");
    cubeShader.addFrag("light_frag.glsl");
    cubeShader.link();

    // Textures

    // Frame timing
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Movement
        glfwPollEvents();
        do_movement(deltaTime);

        // Setup stuff
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glLineWidth(3);
        
        // 4D-3D Transformations
        // Camera
        mat4 rotMatrix4D = camera.GetRotMatrix4D();
        GLfloat w = camera.w;

        // Frustrum 
        vec4 posFrustrum4D(-std::sqrt(3.0f), 0.0f, 1.0f, 0.0f);
        mat4 modelFrustrum4D(1.0f);
        GLfloat cs1 = cos(theta), sn1 = sin(theta);
        modelFrustrum4D[0][0] = cs1;
        modelFrustrum4D[0][3] = -sn1;
        modelFrustrum4D[3][0] = sn1;
        modelFrustrum4D[3][3] = cs1;

        // Tetrahedron
        vec4 posTetrahedron4D(std::sqrt(3.0f), 0.0f, 1.0f, 0.0f);
        mat4 modelTetrahedron4D(1.0f);
        GLfloat cs2 = cos(theta * -0.8f), sn2 = sin(theta * -0.8f);
        modelTetrahedron4D[0][0] = cs2;
        modelTetrahedron4D[0][3] = -sn2;
        modelTetrahedron4D[3][0] = sn2;
        modelTetrahedron4D[3][3] = cs2;

        // Cube
        vec4 posCube4D(0.0f, 0.0f, -2.0f, 0.0f);
        mat4 modelCube4D(1.0f);

        // 3D-2D Transformations
        // Camera
        mat4 model3D(1.0f);
        mat4 view3D = camera.GetViewMatrix();
        mat4 proj3D = glm::perspective(glm::radians(camera.Zoom), 
                                       (float)WIDTH/(float)HEIGHT,
                                       0.1f, 100.0f);
        vec3 camPos = camera.Position;

        // Create Objects
        Object4D hyperFrustrumObj (Object4D::ObjInfo {
            vertices_hyperfrustrum,
            sizeof(vertices_hyperfrustrum)/(4 * sizeof(GL_FLOAT)),
            indices_hyperfrustrum,
            sizeof(indices_hyperfrustrum)/(3 * sizeof(GL_FLOAT)),
            posFrustrum4D,
            modelFrustrum4D,
            model3D,
            proj3D,
            frustrumMat,
            light,
        });

        Object4D hyperTetrahedronObj (Object4D::ObjInfo {
            vertices_hypertetrahedron,
            sizeof(vertices_hypertetrahedron)/(4 * sizeof(GL_FLOAT)),
            indices_hypertetrahedron,
            sizeof(indices_hypertetrahedron)/(3 * sizeof(GL_FLOAT)),
            posTetrahedron4D,
            modelTetrahedron4D,
            model3D,
            proj3D,
            tetrahedronMat,
            light,
        });

        Object4D hyperCubeObj (Object4D::ObjInfo {
            vertices_hypercube,
            sizeof(vertices_hypercube)/(4 * sizeof(GL_FLOAT)),
            indices_hypercube,
            sizeof(indices_hypercube)/(3 * sizeof(GL_FLOAT)),
            posCube4D,
            modelCube4D,
            model3D,
            proj3D,
            cubeMat,
            light,
        });

        // Draw
        hyperFrustrumObj.draw(w, rotMatrix4D, camPos, view3D, frustrumShader);
        hyperTetrahedronObj.draw(w, rotMatrix4D, camPos, view3D, tetrahedronShader);
        hyperCubeObj.draw(w, rotMatrix4D, camPos, view3D, cubeShader);
        
        // Swap the screen buffers*/
        glfwSwapBuffers(window);
    }

    // Terminate GLFW and cleanup
    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
void do_movement(GLfloat deltaTime)
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Change stuff
    if (keys[GLFW_KEY_E])
        camera.ProcessKeyboard(W_ADD, deltaTime);
    if (keys[GLFW_KEY_Q])
        camera.ProcessKeyboard(W_SUB, deltaTime);
    if (keys[GLFW_KEY_1])
        camera.ProcessKeyboard(ROT_DEC, deltaTime);
    if (keys[GLFW_KEY_3])
        camera.ProcessKeyboard(ROT_INC, deltaTime);
    if (keys[GLFW_KEY_2]) {
        camera = Camera(INIT_POS);
        theta = 0.0f;
    }
    if (keys[GLFW_KEY_R])
        theta += glm::radians(0.7f);

    cout << "---------------------" << endl;
    cout << "Position: " << to_string(camera.Position) << endl;
    cout << "W-slice: " << camera.w << endl;
    cout << "ZW Rotation Angle: " << glm::degrees(camera.theta) << endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
        int mode)
{
    cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;  
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}	


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
