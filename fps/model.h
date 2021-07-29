#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QString>
#include "wavefrontobj.h"

struct Transform {
    QQuaternion rotation;
    QVector3D translation;
    float scale;
};

struct Light
{
    QVector4D Position; // 視点座標でのライトの位置
    QVector3D La;       // アンビエント ライト強度
    QVector3D Ld;       // アンビエント ライト強度
    QVector3D Ls;       // スペキュラ ライト強度
};

struct Material
{
    QVector3D Ka;       // アンビエント 反射率
    QVector3D Kd;       // ディフューズ 反射率
    QVector3D Ks;       // スペキュラ 反射率
    float Shininess;    // スペキュラ 輝き係数
};

class Model : protected QOpenGLFunctions
{
public:
    explicit Model();
    virtual ~Model();

    virtual void initialize();
    virtual bool load(const QString &filename);
    virtual void bind(const QString &vertexShader, const QString &fragmentShader);
    virtual void draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix);

    virtual void setRotation(float angleX, float angleY, float angleZ);
    virtual void setTranslation(float x, float y, float z);
    virtual void setScale(float s);
    virtual void setLight(QVector4D position, QVector3D La, QVector3D Ld, QVector3D Ls);
    virtual void setMaterial(QVector3D Ka, QVector3D Kd, QVector3D Ks, float shininess);

    QOpenGLShaderProgram *getShaderProgram() const;
    void setShaderProgram(QOpenGLShaderProgram *shaderProgram);

protected:
    virtual void shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile);
    virtual void bufferInit();

    QOpenGLVertexArrayObject* getVao() const;
    void setVao(QOpenGLVertexArrayObject *vao);

    QVector<QVector3D> getVertices() const;
    void setVertices(const QVector<QVector3D> &vertices);

    QVector<QVector3D> getNormals() const;
    void setNormals(const QVector<QVector3D> &normals);

    QVector<QVector2D> getUvs() const;
    void setUvs(const QVector<QVector2D> &uvs);

    QStringList getComments() const;
    void setComments(const QStringList &comments);

    QOpenGLBuffer getVertex() const;
    void setVertex(const QOpenGLBuffer &vertex);

    QOpenGLBuffer getNormal() const;
    void setNormal(const QOpenGLBuffer &normal);

    QOpenGLBuffer getUv() const;
    void setUv(const QOpenGLBuffer &uv);

private:
    // shader
    QOpenGLShaderProgram* m_shaderProgram;

    // model data
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    QVector<QVector2D> m_uvs;
    QStringList m_comments;

    // buffer
    QOpenGLVertexArrayObject* m_vao;
    QOpenGLBuffer m_vertex;
    QOpenGLBuffer m_normal;
    QOpenGLBuffer m_uv;

    // transform
    Transform m_transform;

    // Lighting
    Light m_light;
    Material m_material;

};

#endif // MODEL_H
