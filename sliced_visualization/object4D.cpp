#include "object4D.h"

Object4D::Object4D() : verts(),
                       model3D(1.0f),
                       VAO(0),
                       VBO(0)
{
}

Object4D::Object4D(ObjInfo info)
{
    setData(info.vertices, info.numVertices, info.indices, info.numTriangles);
    setPos(info.pos);
    setModel4D(info.model4D);
    setModel3D(info.model3D);
    setProj3D(info.proj3D);
    setMaterial(info.mat);
    setLight(info.l);
}

Object4D::~Object4D()
{
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void Object4D::setData(GLfloat *vertexArray, int numVertices,
                       GLuint *indexArray, int numTriangles)
{
    verts = vector<vec4>();
    utils4D::packVector4D(vertexArray, numVertices, verts);
    indices = indexArray;
    this->numTriangles = numTriangles;
}

void Object4D::setPos(vec4 &position)
{
    pos = position;
}

void Object4D::setModel4D(mat4 &model)
{
    model4D = model;
}

void Object4D::setModel3D(mat4 &model)
{
    model3D = model;
}

void Object4D::setProj3D(mat4 &proj)
{
    proj3D = proj;
}

void Object4D::setMaterial(Material &mat)
{
    material = mat;
}

void Object4D::setLight(Light &l)
{
    light = l;
}

void Object4D::prepare(GLfloat w, mat4 &rotMatrix4D)
{
    sliced = vector<GLfloat>();

    // Transform
    vector<vec4> transformed;
    utils4D::transform4D(verts, w, rotMatrix4D, pos, model4D, transformed);

    // Slice
    vector<Point_3> raw;
    utils4D::rawSlice(transformed, indices, numTriangles, 0.0f, raw);
    utils4D::getHull(raw, sliced);
}

void Object4D::buffer()
{
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sliced.size() * sizeof(GL_FLOAT),
                 &sliced[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT),
                          (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT),
                          (GLvoid *)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Object4D::draw(vec3 &camPos, mat4 &view3D, Shader &shad)
{
    shad.use();

    // Lights and Colors (of course, eventually this is texturing)
    GLint objectColorLoc   = glGetUniformLocation(shad.Program, "objectColor");
    GLint lightColorLoc    = glGetUniformLocation(shad.Program, "lightColor");
    GLint viewPosLoc       = glGetUniformLocation(shad.Program, "viewPos");
    GLint matAmbientLoc    = glGetUniformLocation(shad.Program, "material.ambient");
    GLint matDiffuseLoc    = glGetUniformLocation(shad.Program, "material.diffuse");
    GLint matSpecularLoc   = glGetUniformLocation(shad.Program, "material.specular");
    GLint matShineLoc      = glGetUniformLocation(shad.Program, "material.shininess");
    GLint lightPosLoc      = glGetUniformLocation(shad.Program, "light.position");   
    GLint lightAmbientLoc  = glGetUniformLocation(shad.Program, "light.ambient");
    GLint lightDiffuseLoc  = glGetUniformLocation(shad.Program, "light.diffuse");
    GLint lightSpecularLoc = glGetUniformLocation(shad.Program, "light.specular");

    glUniform3f(viewPosLoc, camPos.x, camPos.y, camPos.z);

    glUniform3f(objectColorLoc, material.color.x, material.color.y, material.color.z);
    glUniform3f(matAmbientLoc,  material.ambient.x, material.ambient.y, material.ambient.z);
    glUniform3f(matDiffuseLoc,  material.diffuse.x, material.diffuse.y, material.diffuse.z);
    glUniform3f(matSpecularLoc, material.specular.x, material.specular.y, material.specular.z);
    glUniform1f(matShineLoc,    material.shine);

    glUniform3f(lightPosLoc, camPos.x, camPos.y, camPos.z);
    glUniform3f(lightColorLoc, light.color.x, light.color.y, light.color.z);
    glUniform3f(lightAmbientLoc,  light.ambient.x, light.ambient.y, light.ambient.z);
    glUniform3f(lightDiffuseLoc,  light.diffuse.x, light.diffuse.y, light.diffuse.z);
    glUniform3f(lightSpecularLoc, light.specular.x, light.specular.y, light.specular.z); 

    // Perspective
    GLint viewLoc = glGetUniformLocation(shad.Program, "view3D");
    GLint projectionLoc = glGetUniformLocation(shad.Program, "projection3D");
    GLint modelLoc = glGetUniformLocation(shad.Program, "model3D");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view3D));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj3D));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3D));

    // Draw
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, sliced.size()/6);
    glBindVertexArray(0);
}

void Object4D::draw(GLfloat w, mat4 &rotMatrix4D, vec3 &camPos, mat4 &view3D,
                    Shader &shad)
{
    prepare(w, rotMatrix4D);
    buffer();
    draw(camPos, view3D, shad);
}
