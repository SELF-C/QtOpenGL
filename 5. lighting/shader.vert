#version 400 core
layout(location = 0) in vec3  VertexPosition;
layout(location = 1) in vec3  VertexNormal;

out vec3 LightIntensity;

struct LightInfo {
    vec4 Position;  // 視点座標でのライトの位置
    vec3 La;        // アンビエント ライト強度
    vec3 Ld;        // ディフューズ ライト強度
    vec3 Ls;        // スペキュラ ライト強度
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka;            // アンビエント 反射率
    vec3 Kd;            // ディフューズ 反射率
    vec3 Ks;            // スペキュラ 反射率
    float Shininess;    // スペキュラ 輝き係数
};
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void getEyeSpace( out vec3 norm, out vec4 position )
{
    norm = normalize( NormalMatrix * VertexNormal );
    position = ModelViewMatrix * vec4(VertexPosition, 1.0);
}

vec3 phongModel( vec4 position, vec3 norm )
{
    vec3 s = normalize( vec3(Light.Position - position) );
    vec3 v = normalize( -position.xyz );
    vec3 r = reflect( -s, norm );
    vec3 ambient = Light.La * Material.Ka;
    float sDotN = max( dot(s, norm), 0.0 );
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Ls * Material.Ks * pow( max( dot(r, v), 0.0 ), Material.Shininess );

    return ambient + diffuse + spec;
}

void main(void)
{
    vec3 eyeNorm;
    vec4 eyePosition;

    // 視点空間の位置と法線を取得
    getEyeSpace(eyeNorm, eyePosition);

    // ライティング方程式を評価
    LightIntensity = phongModel( eyePosition, eyeNorm );
    gl_Position = MVP * vec4(VertexPosition, 1.0f);
}
