#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 view3D;
uniform mat4 projection3D;

void main()
{
    gl_Position = projection3D * view3D * vec4(position, 1.0f);
}
