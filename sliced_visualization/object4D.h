#ifndef __OBJECT_4D_H__
#define __OBJECT_4D_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "shader.h"
#include "camera.h"
#include "utils4D.h"

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

using std::vector;
using glm::vec4;
using glm::vec3;
using glm::mat4;
using glm::to_string;
using Point_3 = CGAL::Exact_predicates_inexact_constructions_kernel::Point_3;


class Object4D {
    public:
        struct Material {
            vec3 color;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            GLfloat shine;
        };

        struct Light {
            vec3 color;
            //vec3 pos; For now position is determined by camera position
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
        };

        // Quite a few unsafe patterns here, must refactor to be safe
        // Also some silly groupings (e.g. lights in object?)
        Object4D();
        ~Object4D();
        void setData(GLfloat *vertexArray, int numVertices, 
                     GLuint *indexArray, int numTriangles);
        void setPos(vec4 &position);
        void setModel4D(mat4 &model);
        void setModel3D(mat4 &model);
        void setProj3D(mat4 &proj);
        void setMaterial(Material &mat);
        void setLight(Light &l);
        void prepare(GLfloat w, mat4 &rotMatrix4D);
        void buffer();
        void draw(vec3 &camPos, mat4 &view3D, Shader &shad);

    private:
        vector<vec4> verts;
        GLuint *indices;
        int numTriangles;
        vec4 pos;
        mat4 model4D;
        vector<GLfloat> sliced;
        mat4 model3D;
        mat4 proj3D;
        Material material;
        Light light;
        GLuint VAO;
        GLuint VBO;
};

#endif
