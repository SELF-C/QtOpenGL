#version 400 core
in vec4 qt_Color;
out vec4 FragColor;

void main(void)
{
    FragColor = qt_Color;
}
