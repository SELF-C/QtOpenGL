#version 400 core
layout(location = 0) in vec3  qt_Vertex;
layout(location = 1) in vec3  qt_Normal;

out vec3 qt_LightIntensity;

uniform vec4 qt_LightPosition;
uniform vec3 qt_Kd;
uniform vec3 qt_Ld;

uniform mat4 qt_ModelViewMatrix;
uniform mat3 qt_NormalMatrix;
uniform mat4 qt_ModelViewProjectionMatrix;

void main(void)
{
    vec3 tnorm = normalize( qt_NormalMatrix * qt_Normal);
    vec4 eyeCoords = qt_ModelViewMatrix * vec4(qt_Vertex, 1.0);
    vec3 s = normalize(vec3(qt_LightPosition - eyeCoords));

    qt_LightIntensity = qt_Ld * qt_Kd * max(dot(s, tnorm), 0.0);
    gl_Position = qt_ModelViewProjectionMatrix * vec4(qt_Vertex, 1.0f);
}
