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
const GLuint WIDTH = 1200, HEIGHT = 900;

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

//Light
vec3 lightPos(0.0f, 6.0f, 6.0f);
vec3 lightColor(0.5f, 0.5f, 0.5f);

// Initial w
GLfloat w = 0.0f;

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
                          sizeof(vertices_hyperfrustrum)/sizeof(GL_FLOAT),
                          hyperfrustrum);
    
    vector<vec4> hypertetrahedron;
    utils4D::packVector4D(vertices_hypertetrahedron, 
                          sizeof(vertices_hypertetrahedron)/sizeof(GL_FLOAT),
                          hypertetrahedron);

    // Shader creation
    Shader edgeShader;
    edgeShader.addVert("light_vert.glsl");
    edgeShader.addFrag("light_frag.glsl");
    edgeShader.link();

    // Textures

    // Set up buffer stuff
    GLuint VAO[2], VBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Movement
        glfwPollEvents();
        do_movement();

        // Setup stuff
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glLineWidth(3);

        // 4D-3D Transformations
        // Camera
        mat4 view4D;
        vec4 from(0.0f, 0.0f, 0.0f, 0.0f), to(0.0f, 0.0f, 0.0f, -1.0f); 
        vec4 up(0.0f, 1.0f, 0.0f, 0.0f), over(0.0f, 0.0f, 1.0f, 0.0f);
        view4D = utils4D::lookAt4D(from, to, up, over);

        // Frustrum
        mat4 modelFrustrum4D(1.0f);
        vec4 posFrustrum4D(-1.0f, 0.0f, 0.0f, 0.0f);
        /*GLfloat theta = glm::radians((GLfloat)glfwGetTime() * 50.0f);
        GLfloat cs = cos(theta), sn = sin(theta);
        model4D[0][0] = cs;
        model4D[0][3] = -sn;
        model4D[3][0] = sn;
        model4D[3][3] = cs;*/

        // Tetrahedron
        mat4 modelTetrahedron4D(1.0f);
        vec4 posTetrahedron4D(1.0f, 0.0f, 0.0f, 0.0f);

        // Transform objects
        vector<vec4> transformedFrustrum;
        utils4D::transform4D(hyperfrustrum, from, posFrustrum4D, 
                             modelFrustrum4D, view4D, transformedFrustrum);

        vector<vec4> transformedTetrahedron;
        utils4D::transform4D(hypertetrahedron, from, posTetrahedron4D, 
                             modelTetrahedron4D, view4D, transformedTetrahedron);

        // Get slice
        // Frustrum
        vector<Point_3> raw_hyperfrustrum;
        utils4D::rawSlice(transformedFrustrum, indices_hyperfrustrum,
                          sizeof(indices_hyperfrustrum)/(3 * sizeof(GL_FLOAT)),
                          w, raw_hyperfrustrum);

        vector<GLfloat> sliced_hyperfrustrum;
        utils4D::getHull(raw_hyperfrustrum, sliced_hyperfrustrum);

        // Tetrahedron
        vector<Point_3> raw_hypertetrahedron;
        utils4D::rawSlice(transformedTetrahedron, indices_hypertetrahedron,
                          sizeof(indices_hypertetrahedron)/(3 * sizeof(GL_FLOAT)),
                          w, raw_hypertetrahedron);

        vector<GLfloat> sliced_hypertetrahedron;
        utils4D::getHull(raw_hypertetrahedron, sliced_hypertetrahedron);
        
        // Debug
        cout << "---------------------------------------------" << endl;
        for (auto it = sliced_hyperfrustrum.begin(); 
                  it != sliced_hyperfrustrum.end(); ) {
            cout << "Face: " << endl;
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") ";
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") " << endl;
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") ";
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") " << endl;
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") ";
            cout << "(" << *it++ << ", " << *it++ << ", " << *it++ << ") " << endl;
        }

        // 3D-2D Transformations
        // Camera
        mat4 view3D = camera.GetViewMatrix();
        mat4 proj3D = glm::perspective(glm::radians(camera.Zoom), 
                                       (float)WIDTH/(float)HEIGHT,
                                       0.1f, 100.0f);

        // Load Vertices
        // Frustrum
        glBindVertexArray(VAO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sliced_hyperfrustrum.size() * sizeof(GL_FLOAT),
                     &sliced_hyperfrustrum[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), 
                              (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT),
                              (GLvoid*)(3 * sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        // Tetrahedron
        glBindVertexArray(VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sliced_hypertetrahedron.size() * sizeof(GL_FLOAT),
                     &sliced_hypertetrahedron[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), 
                              (GLvoid*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT),
                              (GLvoid*)(3 * sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        // Set shader uniforms and use shader
        edgeShader.use();

        GLint objectColorLoc   = glGetUniformLocation(edgeShader.Program, "objectColor");
        GLint lightColorLoc    = glGetUniformLocation(edgeShader.Program, "lightColor");
        GLint viewPosLoc       = glGetUniformLocation(edgeShader.Program, "viewPos");
        GLint matAmbientLoc    = glGetUniformLocation(edgeShader.Program, "material.ambient");
        GLint matDiffuseLoc    = glGetUniformLocation(edgeShader.Program, "material.diffuse");
        GLint matSpecularLoc   = glGetUniformLocation(edgeShader.Program, "material.specular");
        GLint matShineLoc      = glGetUniformLocation(edgeShader.Program, "material.shininess");
        GLint lightPosLoc      = glGetUniformLocation(edgeShader.Program, "light.position");   
        GLint lightAmbientLoc  = glGetUniformLocation(edgeShader.Program, "light.ambient");
        GLint lightDiffuseLoc  = glGetUniformLocation(edgeShader.Program, "light.diffuse");
        GLint lightSpecularLoc = glGetUniformLocation(edgeShader.Program, "light.specular");

        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
        glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(matAmbientLoc,  1.0f, 0.5f, 0.31f);
        glUniform3f(matDiffuseLoc,  1.0f, 0.5f, 0.31f);
        glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
        glUniform1f(matShineLoc,    16.0f);
        glUniform3f(lightAmbientLoc,  0.2f, 0.2f, 0.2f);
        glUniform3f(lightDiffuseLoc,  0.5f, 0.5f, 0.5f);
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f); 

        GLint viewLoc = glGetUniformLocation(edgeShader.Program,
                                             "view3D");
        GLint projectionLoc = glGetUniformLocation(edgeShader.Program,
                                                   "projection3D");
        GLint modelLoc = glGetUniformLocation(edgeShader.Program,
                                                "model");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view3D));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj3D));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelFrustrum4D));

        // Draw
        // Frustrum
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, sliced_hyperfrustrum.size()/6);
        glBindVertexArray(0);

        // Tetrahedron
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, sliced_hypertetrahedron.size()/6);

        // Swap the screen buffers*/
        glfwSwapBuffers(window);
    }

    // Terminate GLFW and cleanup
    glfwTerminate();
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
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
        w += 0.05f;
    if(keys[GLFW_KEY_X])
        w -= 0.05f;
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
