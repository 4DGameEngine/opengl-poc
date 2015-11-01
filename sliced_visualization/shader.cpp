#include "shader.h"
Shader::Shader()
{
    Program = glCreateProgram();
}

Shader::~Shader()
{
}

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
{
    this->vertexPath = std::string(vertexPath);
    this->fragmentPath = std::string(fragmentPath);
}

void Shader::addFrag(const GLchar *fragmentPath)
{
    this->fragmentPath = fragmentPath;
}

void Shader::addVert(const GLchar *vertexPath)
{
    this->vertexPath = vertexPath;
}

void Shader::link()
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions (std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::badbit);
    const GLchar *vPath = vertexPath.c_str();
    const GLchar *fPath = fragmentPath.c_str();
    try {
        vShaderFile.open(vPath);
        fShaderFile.open(fPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "Error: Shader: File not successfully read"
            << std::endl;
    }

    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Error: Vertex Shader: Compilation failed\n"
            << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Error: Fragment Shader: Compilation failed\n"
            << infoLog << std::endl;
    }

    Program = glCreateProgram();
    glAttachShader(Program, vertex);
    glAttachShader(Program, fragment);
    glLinkProgram(Program);

    glGetProgramiv(Program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(Program, 512, NULL, infoLog);
        std::cout << "Error: Shader: Linking failed\n"
            << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


void Shader::use()
{
    glUseProgram(Program);
}
