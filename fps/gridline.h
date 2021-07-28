#ifndef GRIDLINE_H
#define GRIDLINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QColor>



class GridLine : protected QOpenGLFunctions
{
public:
    GridLine();
    ~GridLine();

    void draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix);
    void bind(const QString &vertexShader, const QString &fragmentShader);

    void setColor(int red, int green, int blue);

private:
    struct Axis
    {
        int first;
        int count;
    };

    struct Color
    {
        QVector3D grid;
        QVector3D axisX;
        QVector3D axisY;
        QVector3D axisZ;
    };

    // shader
    QOpenGLShaderProgram* m_shaderProgram;

    // mesh data
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_verticesAxisX;

    // buffer
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertex;

    // grid
    float m_width;
    float m_gap;
    Color m_color;
    Axis m_axisX;
    Axis m_axisZ;

    void shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile);
    void bufferInit();
    void gemGridLine();

};

#endif // GRIDLINE_H
