#version 400 core
in vec3 LightIntensity;
in float Opacity;

layout( location = 0 )out vec4 FragColor;

void main(void)
{
    FragColor = vec4(LightIntensity, Opacity);
    //FragColor = vec4(1,1,1, 0.7f);
}
