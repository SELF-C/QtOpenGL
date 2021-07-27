#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QString>
#include "wavefrontobj.h"

struct Light {
    QVector4D Position; // 視点座標でのライトの位置
    QVector3D La;       // アンビエント ライト強度
    QVector3D Ld;       // アンビエント ライト強度
    QVector3D Ls;       // スペキュラ ライト強度
};

struct Material {
    QVector3D Ka;       // アンビエント 反射率
    QVector3D Kd;       // ディフューズ 反射率
    QVector3D Ks;       // スペキュラ 反射率
    float Shininess;    // スペキュラ 輝き係数
};

class Mesh : protected QOpenGLFunctions
{
public:
    explicit Mesh();
    ~Mesh();

    bool load(const QString &filename);
    void bind(const QString &vertexShader, const QString &fragmentShader);
    void draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix);

    void setRotation(float angleX, float angleY, float angleZ);
    void setTranslation(float x, float y, float z);
    void setScale(float s);
    void setLight(QVector4D position, QVector3D La, QVector3D Ld, QVector3D Ls);
    void setMaterial(QVector3D Ka, QVector3D Kd, QVector3D Ks, float shininess);

private:
    // shader
    QOpenGLShaderProgram* m_shaderProgram;

    // mesh data
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    QStringList m_comments;

    // buffer
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertex;
    QOpenGLBuffer m_normal;

    // transform
    QQuaternion m_rotation;
    QVector3D m_translation;
    float m_scale;

    // Lighting
    Light m_light;
    Material m_material;

    void shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile);
    void bufferInit();
};

#endif // MESH_H
