#ifndef GRIDLINE_H
#define GRIDLINE_H

#include <QColor>
#include "model.h"

enum GridColor
{
    Blender,
    Metasequoia,
    Unity,
    UnrealEngine,
};

class GridLine : public Model
{
public:
    GridLine();

    void draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &parentWorldMatrix=QMatrix4x4()) override;
    void release() override;
    void setColor(int red, int green, int blue);
    void setColor(GridColor gridcolor);

private:
    struct VertexData
    {
        QVector3D position;
        QVector3D color;
    };
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

    // buffer
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ibo;

    // vertex data
    QVector<VertexData> m_vertices;
    QVector<GLushort> m_indexes;

    float m_width;
    float m_gap;
    Color m_color;
    Axis m_axisX;
    Axis m_axisZ;

    void bufferInit() override;
    void gemGridLine();

};

#endif // GRIDLINE_H
