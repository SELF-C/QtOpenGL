#version 400 core
layout(location = 0) in vec3  qt_Vertex;
layout(location = 1) in vec3  qt_Color;
out vec3 Color;

uniform mat4 qt_ModelViewProjectionMatrix;

void main(void)
{
    gl_Position = qt_ModelViewProjectionMatrix * vec4(qt_Vertex, 1.0f);
    Color = qt_Color;
}
