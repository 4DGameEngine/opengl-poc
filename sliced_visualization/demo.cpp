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
vec3 INIT_POS(0.0f, 0.0f, 14.0f);
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
    Shader cubeShader;
    cubeShader.addVert("light_vert.glsl");
    cubeShader.addFrag("light_frag.glsl");
    cubeShader.link();

    Shader frustrumShader;
    frustrumShader.addVert("light_vert.glsl");
    frustrumShader.addFrag("light_frag.glsl");
    frustrumShader.link();

    Shader tetrahedronShader;
    tetrahedronShader.addVert("light_vert.glsl");
    tetrahedronShader.addFrag("light_frag.glsl");
    tetrahedronShader.link();

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

        // Positions and transforms
        vector<vec4> positions4D {
            vec4(0.0f, 2.0f, 0.0f, 0.0f), // Ceiling
            vec4(-1.5f, 0.0f, 0.0f, 0.0f), // Wall1L
            vec4(1.5f, 0.0f, 0.0f, 0.0f), // Wall1R
            vec4(0.0f, 0.0f, 0.0f, -4.5f), // WallR1
            vec4(0.0f, 0.0f, 0.0f, -1.5f), // WallR2
            vec4(0.0f, 0.0f, 0.0f, 1.5f), // WallR3
            vec4(0.0f, 0.0f, 0.0f, 4.5f), // WallR4
            vec4(0.0f, 0.0f, 0.0f, 0.0f), // Cube
        };

        vector<mat4> models4D(positions4D.size(), mat4(1.0f));

        models4D[0][0][0] = 2.0f;
        models4D[0][1][1] = 0.5f;
        models4D[0][3][3] = 5.0f;

        models4D[1][0][0] = 0.5f;
        models4D[1][1][1] = 1.5f;
        models4D[1][3][3] = 5.0f;

        models4D[2][0][0] = 0.5f;
        models4D[2][1][1] = 1.5f;
        models4D[2][3][3] = 5.0f;

        models4D[3][0][0] = 5.0f;
        models4D[3][1][1] = 1.5f;
        models4D[3][3][3] = 0.5f;

        models4D[4][0][0] = 5.0f;
        models4D[4][1][1] = 1.5f;
        models4D[4][3][3] = 0.5f;

        models4D[5][0][0] = 5.0f;
        models4D[5][1][1] = 1.5f;
        models4D[5][3][3] = 0.5f;

        models4D[6][0][0] = 5.0f;
        models4D[6][1][1] = 1.5f;
        models4D[6][3][3] = 0.5f;

        GLfloat cs = cos(glm::radians(glfwGetTime() * 50.0f));
        GLfloat sn = sin(glm::radians(glfwGetTime() * 50.0f));
        models4D[7][0][0] = cs * 0.6f;
        models4D[7][2][0] = -sn;
        models4D[7][0][2] = sn;
        models4D[7][2][2] = cs * 0.6f;
        models4D[7][1][1] = cs * 0.6f;
        models4D[7][3][1] = -sn;
        models4D[7][1][3] = sn;
        models4D[7][3][3] = cs * 0.6f;

        // Frustrum 
        vec4 posFrustrum4D(0.0f, 0.0f, 0.0f, -3.0f);
        mat4 modelFrustrum4D(1.0f);
        modelFrustrum4D[1][1] = cs;
        modelFrustrum4D[2][1] = -sn;
        modelFrustrum4D[1][2] = sn;
        modelFrustrum4D[2][2] = cs;
        modelFrustrum4D[0][0] = cs;
        modelFrustrum4D[3][0] = -sn;
        modelFrustrum4D[0][3] = sn;
        modelFrustrum4D[3][3] = cs;

        // Tetrahedron
        vec4 posTetrahedron4D(0.0, 0.0f, 0.0f, 3.0f);
        mat4 modelTetrahedron4D(1.0f);
        modelTetrahedron4D[0][0] = cs;
        modelTetrahedron4D[1][0] = -sn;
        modelTetrahedron4D[0][1] = sn;
        modelTetrahedron4D[1][1] = cs;
        modelTetrahedron4D[2][2] = cs;
        modelTetrahedron4D[3][2] = -sn;
        modelTetrahedron4D[2][3] = sn;
        modelTetrahedron4D[3][3] = cs;

        // 3D-2D Transformations
        // Camera
        mat4 model3D(1.0f);
        mat4 view3D = camera.GetViewMatrix();
        mat4 proj3D = glm::perspective(glm::radians(camera.Zoom), 
                                       (float)WIDTH/(float)HEIGHT,
                                       0.1f, 100.0f);
        vec3 camPos = camera.Position;

        // Create Objects
        Object4D cubeObj (Object4D::ObjInfo {
            vertices_hypercube,
            sizeof(vertices_hypercube)/(4 * sizeof(GL_FLOAT)),
            indices_hypercube,
            sizeof(indices_hypercube)/(3 * sizeof(GL_FLOAT)),
            vec4(),
            mat4(),
            model3D,
            proj3D,
            cubeMat,
            light,
        });

        // Draw
        for (size_t i = 0; i < positions4D.size(); i++) {
            cubeObj.setPos(positions4D[i]);
            cubeObj.setModel4D(models4D[i]);
            cubeObj.draw(w, rotMatrix4D, camPos, view3D, cubeShader);
        }

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

        hyperFrustrumObj.draw(w, rotMatrix4D, camPos, view3D, frustrumShader);

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

        hyperTetrahedronObj.draw(w, rotMatrix4D, camPos, view3D, tetrahedronShader);

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
