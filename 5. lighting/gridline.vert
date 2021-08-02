#version 400 core
layout(location = 0) in vec3  VertexPosition;

out vec3 Color;

uniform vec3 VertexColor;
uniform mat4 MVP;

void main(void)
{
    Color = VertexColor;
    gl_Position = MVP * vec4(VertexPosition, 1.0f);
}
