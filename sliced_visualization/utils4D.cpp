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

std::map<cvec3, std::set<cvec3> > *utils4D::takeSlice(const GLfloat vertices[], 
                                                      int numVertices,
                                                      const GLuint indices[], 
                                                      int numTriangles,
                                                      GLfloat w)
{
    // Switch to unordered maps and sets eventually?
    map<cvec3, set<cvec3> > *intersects = new map<cvec3, set<cvec3> >;
    vector<vec4> verticesVector;
    populateVector(verticesVector, vertices, numVertices, 4);

    for (int i = 0; i < numTriangles; i++) {
        // Orientation is irrelevant here, we orient the slice according to
        // what is most 'natural' in 3d.
        vec4 point0 = verticesVector[indices[3*i]];
        vec4 point1 = verticesVector[indices[3*i+1]];
        vec4 point2 = verticesVector[indices[3*i+2]];

        bool flag0, flag1, flag2;
        cvec3 intersect0(intersectLine(point0, point1, w, flag0));
        cvec3 intersect1(intersectLine(point1, point2, w, flag1));
        cvec3 intersect2(intersectLine(point2, point0, w, flag2));

        if (flag0 && flag1) {
            (*intersects)[intersect0].insert(intersect1);
            (*intersects)[intersect1].insert(intersect0);
        }
        if (flag1 && flag2) {
            (*intersects)[intersect1].insert(intersect2);
            (*intersects)[intersect2].insert(intersect1);
        }
        if (flag2 && flag0) {
            (*intersects)[intersect2].insert(intersect0);
            (*intersects)[intersect0].insert(intersect2);
        }
    }

    return intersects;
}

void utils4D::populateVector(vector<vec4> &verticesVector, 
                             const GLfloat vertices[],
                             int numVertices, int dimensions)
{
    for (int i = 0; i < numVertices; i++) {
        verticesVector.push_back(vec4(vertices[dimensions*i],
                                      vertices[dimensions*i+1],
                                      vertices[dimensions*i+2],
                                      vertices[dimensions*i+3]));
    }
}

// Might want to change this to exact match for now?
glm::vec4 utils4D::intersectLine(const vec4 &start, const vec4 &end, int w, 
                                 bool &success)
{
    // Don't know if this should be more robust
    if (fabs(end.w - start.w) < FLOAT_TOLERANCE) {
        if (fabs(w - start.w) < FLOAT_TOLERANCE) {
            success = true;
            return start;
        } else {
            success = false;
            return vec4();
        }
    }

    GLfloat ratio = (w - start.w)/(end.w - start.w);
    if (ratio < -FLOAT_TOLERANCE || ratio > 1 + FLOAT_TOLERANCE) {
        success = false;
        return vec4();
    } else {
        success = true;
        return start + ratio * (end - start);
    }
}
