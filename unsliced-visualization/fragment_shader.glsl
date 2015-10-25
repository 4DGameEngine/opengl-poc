#version 330 core

in vec4 vertCol;
out vec4 color;

void main()
{
    if (abs(vertCol.w + 4.0f) > 0.01f) discard;
    else color = vec4(vertCol.x, vertCol.y, vertCol.z, 1.0f);

    
