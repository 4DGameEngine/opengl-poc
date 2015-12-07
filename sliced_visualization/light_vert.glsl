#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model3D;
uniform mat4 view3D;
uniform mat4 projection3D;

void main()
{
    gl_Position = projection3D * view3D * model3D * vec4(position, 1.0f);
    Normal = mat3(transpose(inverse(model3D))) * normal;
    FragPos = vec3(model3D * vec4(position, 1.0));
} 
