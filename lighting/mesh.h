#ifndef MESH_H
#define MESH_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QString>
#include "wavefrontobj.h"

class Mesh : protected QOpenGLFunctions
{
public:
    explicit Mesh();

    bool load(const QString &filename);
    void bind(const QString &vertexShader, const QString &fragmentShader);
    void draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix);

    void setRotation(float angleX, float angleY, float angleZ);
    void setTranslation(float x, float y, float z);
    void setScale(float s);
    void setLightPostion(float x, float y, float z);
    void setKd(float r, float g, float b);
    void setLd(float r, float g, float b);

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
    QVector4D m_lightPosition;
    QVector3D m_kd;
    QVector3D m_ld;

    void shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile);
    void bufferInit();
};

#endif // MESH_H
