#version 400 core
layout(location = 0) in vec3  qt_Vertex;

out vec3 Color;

uniform vec3 qt_Color;

void main(void)
{
    gl_Position = vec4(qt_Vertex, 1.0);
    Color = qt_Color;
}
