#ifndef _UTILS4D_H_
#define _UTILS4D_H_

#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <GLFW/glfw3.h>

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
    using Point_3 = CGAL::Exact_predicates_inexact_constructions_kernel::Point_3;

    const GLfloat FLOAT_TOLERANCE = 0.001f;

    mat4 lookAt4D(const vec4 &from, const vec4 &to, const vec4 &up, 
                  const vec4 &over);
    vec4 cross4D(const vec4 &v0, const vec4 &v1, const vec4 &v2);

    void rawSlice(vector<vec4> vertices, const GLuint indices[], int numTriangles,
                  GLfloat w, vector<Point_3> &intersects);
    void getHull(const vector<Point_3> &raw, vector<GLfloat> &buf);

    vec4 intersectLine(const vec4 &start, const vec4 &end, const GLfloat w,
                       bool &success);
}

#endif
