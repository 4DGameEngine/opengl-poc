#include "shader.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

// GLEW
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

// GLFW
#include <GLFW/glfw3.h>

// SOIL2
#include <SOIL2.h>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);
glm::mat4 lookAt4D(const glm::vec4 &from, const glm::vec4 &to, 
                   const glm::vec4 &up, const glm::vec4 &over);
glm::vec4 cross4D(const glm::vec4 &v0, const glm::vec4 &v1,
                  const glm::vec4 &v2);
glm::mat4x3 proj4D();
template <class T>
void printMat(const T &m, int row, int col);
template <class T>
void printVec(const T &v, int col);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// The MAIN function, from here we start the application and run the 
// game loop
int main()
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // OS X requests 3.3 differently.
#ifdef __APPLEOS__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", 
                                          nullptr, nullptr);    
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving
    // function pointers and extensions
    // More hacking for my autocomplete.
#ifndef CLANG_COMPLETE_ONLY
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
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
    
    // Shader creation
    Shader simpleShader("vertex_shader.glsl", "fragment_shader.glsl");

    // Data
    GLfloat vertices[] = {
        // Positions           
         0.5f,   0.5f,   0.5f,   0.5f,
         0.5f,  -0.5f,   0.5f,   0.5f,  
        -0.5f,  -0.5f,   0.5f,   0.5f,
        -0.5f,   0.5f,   0.5f,   0.5f,

         0.5f,   0.5f,  -0.5f,   0.5f,
         0.5f,  -0.5f,  -0.5f,   0.5f,
        -0.5f,  -0.5f,  -0.5f,   0.5f,
        -0.5f,   0.5f,  -0.5f,   0.5f,

         0.5f,   0.5f,   0.5f,  -0.5f,
         0.5f,  -0.5f,   0.5f,  -0.5f,  
        -0.5f,  -0.5f,   0.5f,  -0.5f,
        -0.5f,   0.5f,   0.5f,  -0.5f,

         0.5f,   0.5f,  -0.5f,  -0.5f,
         0.5f,  -0.5f,  -0.5f,  -0.5f,
        -0.5f,  -0.5f,  -0.5f,  -0.5f,
        -0.5f,   0.5f,  -0.5f,  -0.5f
    };

    GLuint indices[] = {
        // Front Cube
         0,  1,  2,
         0,  2,  3,
         4,  0,  3,
         4,  3,  7,
         4,  5,  1,
         4,  1,  0,
         3,  2,  6,
         3,  6,  7,
         1,  5,  6,
         1,  6,  2,
         7,  6,  5,
         7,  5,  4,
        // Back Cube
         8,  9, 10,
         8, 10, 11,
        12,  8, 11,
        12, 11, 15,
        12, 13,  9,
        12,  9,  8,
        11, 10, 14,
        11, 14, 15,
         9, 13, 14,
         9, 14, 10,
        15, 14, 13,
        15, 13, 12,
        // Top Cube
         0,  4,  7,
         0,  7,  3,
        12,  0,  3,
        12,  3, 15,
        12,  8,  4,
        12,  4,  0,
         3,  7, 11,
         3, 11, 15,
         4,  8, 11,
         4, 11,  7,
        15, 11,  8,
        15,  8, 12,
        // Bottom Cube
         5,  1,  2,
         5,  2,  6,
         9,  5,  6,
         9,  6, 10,
         9, 13,  1,
         9,  1,  5,
         6,  2, 14,
         6, 14, 10,
         1, 13, 14,
         1, 14,  2,
        10, 14, 13,
        10, 13,  9,
        // Left Cube
         7,  6,  2,
         7,  2,  3,
        11,  7,  3,
        11,  3, 15,
        11, 10,  6,
        11,  6,  7,
         3,  2, 14,
         3, 14, 15,
         6, 10, 14,
         6, 14,  2,
        15, 14, 10,
        15, 10, 11,
        // Right Cube
         0,  1,  5,
         0,  5,  4,
        12,  0,  4,
        12,  4,  8,
        12, 13,  1,
        12,  1,  0,
         4,  5,  9,
         4,  9,  8,
         1, 13,  9,
         1,  9,  5,
         8,  9, 13,
         8, 13, 12
    };

    // Textures

    // Set up buffer stuff
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved
        // etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simpleShader.Use();

        // 4D-3D Transformations
        glm::mat4 model4D(1.0f), view4D;
        GLfloat theta = glm::radians((GLfloat)glfwGetTime() * 50.0f);
        /*GLfloat cs = cos(theta), sn = sin(theta);
        model4D[0][0] = cs;
        model4D[0][1] = -sn;
        model4D[1][0] = sn;
        model4D[1][1] = cs;*/
        glm::mat4x3 projection4D;
        glm::vec4 from(4.0f, 0.0f, 0.0f, 0.0f), to(0.0f, 0.0f, 0.0f, 0.0f); 
        glm::vec4 up(0.0f, 1.0f, 0.0f, 0.0f), over(0.0f, 0.0f, 1.0f, 0.0f);
        //assert(cross4D(glm::vec4(1,0,0,0), glm::vec4(0,1,0,0), glm::vec4(0,0,1,0)) == glm::vec4(0,0,0,1));
        view4D = lookAt4D(from, to, up, over);
        std::cout << "Proj Mat: " << std::endl;
        printMat(view4D, 4, 4);
        projection4D = proj4D();

        GLfloat *projVert = new GLfloat[48];
        std::cout << "--------------------------------" << std::endl;
        for(int i = 0; i != 16; i++) {
            // Project each vertex to the 3D space
            glm::vec4 vert4(vertices[i*4], vertices[i*4+1],
                            vertices[i*4+2], vertices[i*4+3]);
            glm::vec4 viewVert = view4D * (model4D * vert4 - from); 
            printVec(viewVert, 4);
            glm::vec3 vert3 = projection4D * view4D * (model4D * vert4 - from);
            projVert[i*3] = vert3.x;
            projVert[i*3+1] = vert3.y;
            projVert[i*3+2] = vert3.z;
        }

        // 3D-2D Transformations
        glm::mat4 view3D = glm::lookAt(glm::vec3(3.0f, 0.99f, 1.82f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj3D = glm::perspective(glm::radians(45.0f),
                                            (float)WIDTH/(float)HEIGHT,
                                            0.1f, 100.0f);

        GLint viewLoc = glGetUniformLocation(simpleShader.Program,
                                             "view3D");
        GLint projectionLoc = glGetUniformLocation(simpleShader.Program,
                                                   "projection3D");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view3D));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj3D));

        // Load Vertices
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 48*sizeof(GL_FLOAT), projVert, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, 
                     GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), 
                              (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 216, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);

        delete[] projVert;
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    return 0;
}

glm::mat4 lookAt4D(const glm::vec4 &from, const glm::vec4 &to,
                   const glm::vec4 &up, const glm::vec4 &over)
{
    glm::mat4 viewMat;
    glm::vec4 A, B, C, D;
    D = glm::normalize(from - to);
    std::cout << "D ";
    printVec(D, 4);
    std::cout << "A ";
    A = glm::normalize(cross4D(up, over, D));
    std::cout << "B ";
    B = glm::normalize(cross4D(over, D, A));
    std::cout << "C ";
    C = cross4D(D, A, B);
    viewMat[0] = A;
    viewMat[1] = B;
    viewMat[2] = C;
    viewMat[3] = D;
    // I hope this preserves "right-handedness"
    //assert(glm::normalize(cross4D(A, B, C)) == D);
    return glm::transpose(viewMat);
}

glm::vec4 cross4D(const glm::vec4 &v0, const glm::vec4 &v1,
                  const glm::vec4 &v2)
{
    glm::vec4 crossVec;
    glm::mat3 tempMat;
    std::cout << "Vectors: " << std::endl;
    printVec(v0, 4);
    printVec(v1, 4);
    printVec(v2, 4);
    tempMat[0] = glm::vec3(v0.y, v0.z, v0.w);
    tempMat[1] = glm::vec3(v1.y, v1.z, v1.w);
    tempMat[2] = glm::vec3(v2.y, v2.z, v2.w);
    crossVec.x = glm::determinant(tempMat);
    printMat(tempMat, 3, 3);
    tempMat[0] = glm::vec3(v0.x, v0.z, v0.w);
    tempMat[1] = glm::vec3(v1.x, v1.z, v1.w);
    tempMat[2] = glm::vec3(v2.x, v2.z, v2.w);
    crossVec.y = -glm::determinant(tempMat);
    printMat(tempMat, 3, 3);
    tempMat[0] = glm::vec3(v0.x, v0.y, v0.w);
    tempMat[1] = glm::vec3(v1.x, v1.y, v1.w);
    tempMat[2] = glm::vec3(v2.x, v2.y, v2.w);
    crossVec.z = glm::determinant(tempMat);
    printMat(tempMat, 3, 3);
    tempMat[0] = glm::vec3(v0.x, v0.y, v0.z);
    tempMat[1] = glm::vec3(v1.x, v1.y, v1.z);
    tempMat[2] = glm::vec3(v2.x, v2.y, v2.z);
    crossVec.w = -glm::determinant(tempMat);
    printMat(tempMat, 3, 3);
    printVec(crossVec, 4);
    assert(glm::length(crossVec) >= 0.001);
    return crossVec;
}

glm::mat4x3 proj4D()
{
    // Infinite draw distance for now
    // Also draws things behind, not ideal
    glm::mat4x3 retMat(1);
    return retMat;
}

template <class T> 
void printMat(const T &m, int col, int row)
{
    std::cout << "-----------" << std::endl;
    for (int rw = 0; rw < row; rw++) {
        std::cout << "|";
        for (int cl = 0; cl < col; cl++)
            std::cout << " " << m[cl][rw];
        std::cout << " |" << std::endl;
    }
    std::cout << "-----------" << std::endl;
}

template <class T>
void printVec(const T &v, int col)
{
    std::cout << "Vec:  ";
    for (int i = 0; i < col; i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
