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
const GLuint WIDTH = 800, HEIGHT = 600;

// Function prototypes
void do_movement();
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//Camera
Camera camera(vec3(0.0f, 0.0f, 0.2f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Initial w
GLfloat w = -4.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

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

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", 
                                          nullptr, nullptr);    
    if (window == nullptr)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
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
    
    // Data
    GLfloat vertices[] = {
        // Positions           
         0.6f,   0.6f,   0.6f,   0.6f,
         0.6f,  -0.6f,   0.6f,   0.6f,  
        -0.6f,  -0.6f,   0.6f,   0.6f,
        -0.6f,   0.6f,   0.6f,   0.6f,

         0.6f,   0.6f,  -0.6f,   0.6f,
         0.6f,  -0.6f,  -0.6f,   0.6f,
        -0.6f,  -0.6f,  -0.6f,   0.6f,
        -0.6f,   0.6f,  -0.6f,   0.6f,

         0.6f,   0.6f,   0.6f,  -0.6f,
         0.6f,  -0.6f,   0.6f,  -0.6f,  
        -0.6f,  -0.6f,   0.6f,  -0.6f,
        -0.6f,   0.6f,   0.6f,  -0.6f,

         0.6f,   0.6f,  -0.6f,  -0.6f,
         0.6f,  -0.6f,  -0.6f,  -0.6f,
        -0.6f,  -0.6f,  -0.6f,  -0.6f,
        -0.6f,   0.6f,  -0.6f,  -0.6f,
    };

    GLuint indices[] = {
         0,  1,  2,
         0,  2,  3,
         0,  5,  1,
         0,  4,  5,
         0,  3,  7,
         0,  7,  4,

         6,  1,  5,
         6,  2,  1,
         6,  3,  2,
         6,  7,  3,
         6,  5,  4,
         6,  7,  4,

         9, 10, 11,
         9, 11,  8,
         9,  8, 12,
         9, 12, 13,
         9, 14, 10,
         9, 13, 14,

        15, 12,  8,
        15,  8, 11,
        15, 14, 13,
        15, 13, 12,
        15, 10, 14,
        15, 11, 10,

         3,  8,  0,
         3, 11,  8,
         3, 11, 10,
         3, 10,  2,
         3, 15, 11,
         3,  7, 15,
         
        14,  6,  2,
        14,  2, 10,
        14,  5,  6,
        14, 13,  5,
        14,  7,  6,
        14, 15,  7,

         1,  8,  0,
         1,  9,  8,
         1,  5, 13,
         1, 13,  9,
         1,  2, 10,
         1, 10,  9,

        12,  4,  0,
        12,  0,  8,
        12,  4,  5,
        12,  5, 13,
        12,  7,  4,
        12, 15,  7,
    };

    // Pack data into vector
    vector<vec4> hypercube;
    for (int i = 0; i != 16; i++) {
        hypercube.push_back(vec4(vertices[i*4], vertices[i*4+1],
                                 vertices[i*4+2], vertices[i*4+3]));
    }
    


    // Shader creation
    Shader edgeShader;
    edgeShader.addVert("vertex_shader.glsl");
    edgeShader.addFrag("fragment_shader.glsl");
    edgeShader.link();

    // Textures

    // Set up buffer stuff
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    while (!glfwWindowShouldClose(window))
    {
        //set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        do_movement();

        // Setup stuff
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glLineWidth(3);

        // 4D-3D Transformations
        mat4 model4D(1.0f), view4D;
        GLfloat theta = glm::radians((GLfloat)glfwGetTime() * 50.0f);
        GLfloat cs = cos(theta), sn = sin(theta);
        model4D[0][0] = cs;
        model4D[0][3] = -sn;
        model4D[3][0] = sn;
        model4D[3][3] = cs;
        vec4 from(0.0f, 0.0f, 0.0f, 4.0f), to(0.0f, 0.0f, 0.0f, 0.0f); 
        vec4 up(0.0f, 1.0f, 0.0f, 0.0f), over(0.0f, 0.0f, 1.0f, 0.0f);
        view4D = utils4D::lookAt4D(from, to, up, over);

        // Transform each vertex
        vector<vec4> transformedCube;
        for(int i = 0; i != 16; i++) {
            vec4 transformedVert = view4D * (hypercube[i] - from); 
            transformedCube.push_back(std::move(transformedVert));
        }

        // Get slice
        vector<Point_3> raw;
        utils4D::rawSlice(transformedCube, indices, 48, w, raw);

        vector<GLfloat> sliced;
        utils4D::getHull(raw, sliced);
        
        cout << "---------------------------------------------" << endl;
        for (auto it = sliced.begin(); it != sliced.end(); ) {
            cout << "Face: ";
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") ";
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") ";
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ")" << endl;
        }

        // 3D-2D Transformations
        mat4 view3D = camera.GetViewMatrix();
        mat4 proj3D = glm::perspective(glm::radians(camera.Zoom), 
                                       (float)WIDTH/(float)HEIGHT,
                                       0.1f, 100.0f);
        // Load Vertices
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sliced.size() * sizeof(GL_FLOAT),
                     &sliced[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), 
                              (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        // Set shader uniforms and use shader
        edgeShader.use();

        GLint viewLoc = glGetUniformLocation(edgeShader.Program,
                                             "view3D");
        GLint projectionLoc = glGetUniformLocation(edgeShader.Program,
                                                   "projection3D");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view3D));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj3D));

        // Draw
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, sliced.size()/3);
        glBindVertexArray(0);

        // Swap the screen buffers*/
        glfwSwapBuffers(window);
    }

    // Terminate GLFW and cleanup
    glfwTerminate();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    return 0;
}
// Moves/alters the camera positions based on user input
void do_movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    // Change w
    if(keys[GLFW_KEY_Z])
        w += 0.1f;
    if(keys[GLFW_KEY_X])
        w -= 0.1f;
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
