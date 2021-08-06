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

class Model : protected QOpenGLFunctions
{
public:
    struct VertexData
    {
        QVector3D position;
        QVector3D normal;
        QVector2D texCoord;
        int getPositionOffset(){ return 0;}
        int getNormalOffset(){ return sizeof(QVector3D);}
        int getTexCoordOffset(){ return sizeof(QVector3D) + sizeof(QVector2D);}
    };

    struct Transform
    {
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

    explicit Model();
    virtual ~Model();

    virtual void initialize();
    virtual void release();
    virtual bool load(const QString &filename);
    virtual void bind(const QString &vertexShader, const QString &fragmentShader);
    virtual void draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix);

    virtual void setRotation(float angleX, float angleY, float angleZ);
    virtual void setTranslation(float x, float y, float z);
    virtual void setScale(float s);
    virtual void setLight(QVector4D position, QVector3D La, QVector3D Ld, QVector3D Ls);
    virtual void setMaterial(QVector3D Ka, QVector3D Kd, QVector3D Ks, float shininess);


    QVector<VertexData> getVertices() const;
    void setVertices(const QVector<VertexData> &vertices);
    Light getLight() const;
    Material getMaterial() const;


protected:
    QOpenGLShaderProgram *getShaderProgram() const;
    void setShaderProgram(QOpenGLShaderProgram *shaderProgram);
    QOpenGLBuffer getVbo() const;
    void setVbo(const QOpenGLBuffer &vbo);
    QOpenGLBuffer getIbo() const;
    void setIbo(const QOpenGLBuffer &ibo);
    QStringList getComments() const;
    void setComments(const QStringList &comments);


private:
    // shader
    QOpenGLShaderProgram* m_shaderProgram;

    // Vertex data
    QVector<VertexData> m_vertices;
    QVector<GLushort> m_indexes;
    QStringList m_comments;

    // buffer
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ibo;

    // transform
    Transform m_transform;

    // Lighting
    Light m_light;
    Material m_material;

    // status
    bool m_created;

    virtual void shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile);
    virtual void bufferInit();
};

#endif // MODEL_H
