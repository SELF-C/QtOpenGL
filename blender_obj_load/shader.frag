#version 400 core
in vec3 qt_LightIntensity;
layout( location = 0 )out vec4 FragColor;

void main(void)
{
    FragColor = vec4(qt_LightIntensity, 1.0f);
}
