#include <iostream>
#include <utility>

#include <cmath>
#include <cassert>

// CGAL headers are a bit picky about which order you include them in
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

#include "utils4D.h"

glm::mat4 utils4D::lookAt4D(const vec4 &from, const vec4 &to, const vec4 &up, 
                           const vec4 &over)
{
    mat4 viewMat;
    vec4 A, B, C, D;
    D = glm::normalize(from - to);
    A = glm::normalize(cross4D(up, over, D));
    B = glm::normalize(cross4D(over, D, A));
    C = cross4D(D, A, B);
    viewMat[0] = A;
    viewMat[1] = B;
    viewMat[2] = C;
    viewMat[3] = D;
    return glm::transpose(viewMat);
}

glm::vec4 utils4D::cross4D(const vec4 &v0, const vec4 &v1, const vec4 &v2)
{
    vec4 crossVec;
    glm::mat3 tempMat;
    tempMat[0] = vec3(v0.y, v0.z, v0.w);
    tempMat[1] = vec3(v1.y, v1.z, v1.w);
    tempMat[2] = vec3(v2.y, v2.z, v2.w);
    crossVec.x = glm::determinant(tempMat);
    tempMat[0] = vec3(v0.x, v0.z, v0.w);
    tempMat[1] = vec3(v1.x, v1.z, v1.w);
    tempMat[2] = vec3(v2.x, v2.z, v2.w);
    crossVec.y = -glm::determinant(tempMat);
    tempMat[0] = vec3(v0.x, v0.y, v0.w);
    tempMat[1] = vec3(v1.x, v1.y, v1.w);
    tempMat[2] = vec3(v2.x, v2.y, v2.w);
    crossVec.z = glm::determinant(tempMat);
    tempMat[0] = vec3(v0.x, v0.y, v0.z);
    tempMat[1] = vec3(v1.x, v1.y, v1.z);
    tempMat[2] = vec3(v2.x, v2.y, v2.z);
    crossVec.w = -glm::determinant(tempMat);
    assert(glm::length(crossVec) >= 0.001f);
    return crossVec;
}

void utils4D::rawSlice(vector<vec4> vertices, const GLuint indices[], int numTriangles,
                       GLfloat w, vector<Point_3> &intersects)
{
    // Calculate intersections with each triangle
    for (int i = 0; i < numTriangles; i++) {
        vec4 point0 = vertices[indices[3*i]];
        vec4 point1 = vertices[indices[3*i+1]];
        vec4 point2 = vertices[indices[3*i+2]];

        bool flag0, flag1, flag2;
        vec3 int0(intersectLine(point0, point1, w, flag0));
        vec3 int1(intersectLine(point1, point2, w, flag1));
        vec3 int2(intersectLine(point2, point0, w, flag2));

        if (flag0)
            intersects.push_back(Point_3(int0.x, int0.y, int0.z));
        if (flag1)
            intersects.push_back(Point_3(int1.x, int1.y, int1.z));
        if (flag2)
            intersects.push_back(Point_3(int2.x, int2.y, int2.z));
    }
}

void utils4D::getHull(const vector<Point_3> &raw, vector<GLfloat> &buf)
{
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Polyhedron = CGAL::Polyhedron_3<Kernel>;

    // Return if no hull possible
    if (raw.size() < 3) return;

    // Get convex hull
    Polyhedron poly;
    CGAL::convex_hull_3(raw.begin(), raw.end(), poly);

    // Circulate around each facet to retrieve coordinates
    for (auto fit = poly.facets_begin(); fit != poly.facets_end(); fit++) {
        auto hit = fit->facet_begin(), orig = hit;
        CGAL_assertion(CGAL::circulator_size(hit) == 3);
        
        do {
            auto &v0 = (hit++)->vertex()->point(), 
                 &v1 = (hit++)->vertex()->point(),
                 &v2 = (hit++)->vertex()->point();

            auto n = CGAL::cross_product(v1 - v0, v2 - v0);
            
            // Lol wat
            buf.push_back(v0.x());
            buf.push_back(v0.y());
            buf.push_back(v0.z());
            buf.push_back(n.x());
            buf.push_back(n.y());
            buf.push_back(n.z());
            buf.push_back(v1.x());
            buf.push_back(v1.y());
            buf.push_back(v1.z());
            buf.push_back(n.x());
            buf.push_back(n.y());
            buf.push_back(n.z());
            buf.push_back(v2.x());
            buf.push_back(v2.y());
            buf.push_back(v2.z());
            buf.push_back(n.x());
            buf.push_back(n.y());
            buf.push_back(n.z());

        } while (hit != orig);
    }
}

// Might want to change this to exact match for now?
glm::vec4 utils4D::intersectLine(const vec4 &start, const vec4 &end, 
                                 const GLfloat w, bool &success)
{
    // Don't know if this should be more robust
    // fabsf should be right?
    if (end.w == start.w) {
        if (w == start.w) {
            success = true;
            return start;
        } else {
            success = false;
            return vec4();
        }
    }

    GLfloat ratio = (w - start.w)/(end.w - start.w);
    //if (ratio < -FLOAT_TOLERANCE || ratio > 1.0f + FLOAT_TOLERANCE) {
    if (ratio < 0 || ratio > 1.0f) {
        success = false;
        return vec4();
    } else {
        success = true;
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        return start + ratio * (end - start);
    }
}
