#version 400 core
in vec3 LightIntensity;

layout( location = 0 )out vec4 FragColor;

void main(void)
{
    FragColor = vec4(LightIntensity, 1.0f);
    //FragColor = vec4(1,1,1, 0.7f);
}
