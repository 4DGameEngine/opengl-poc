#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

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

class Shader
{
    public:
        GLuint Program;
        Shader();
        Shader(const GLchar *vertexPath, const GLchar *fragmentPath);
        ~Shader();
        void addFrag(const GLchar *fragmentPath);
        void addVert(const GLchar *vertexPath);
        void link();
        void use();
        void test() {};
    private:
        std::string fragmentPath;
        std::string vertexPath;
};

#endif
