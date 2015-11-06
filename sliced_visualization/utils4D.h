#ifndef _UTILS4D_H_
#define _UTILS4D_H_

#include <vector>
#include <map>
#include <set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include "cvec3.h"

/* utils4D.h
 *
 * Defines the utils4D namespaces which contains functions for 4D manipulation
 */
namespace utils4D 
{
    using glm::mat4;
    using glm::vec3;
    using glm::vec4;
    using std::vector;
    using std::map;
    using std::set;

    const GLfloat FLOAT_TOLERANCE = 0.001f;

    mat4 lookAt4D(const vec4 &from, const vec4 &to, const vec4 &up, 
                  const vec4 &over);
    vec4 cross4D(const vec4 &v0, const vec4 &v1, const vec4 &v2);

    map<cvec3, set<cvec3> > *takeSlice(const GLfloat vertices[], 
                                       int numVertices, 
                                       const GLuint indices[], 
                                       int numTriangles,  
                                       GLfloat w);

    void populateVector(vector<vec4> &verticesVector, const GLfloat vertices[],
                        int numVertices, int dimensions);
    vec4 intersectLine(const vec4 &start, const vec4 &end, int w, 
                       bool &success);
}

#endif
