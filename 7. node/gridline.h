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

    void draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix) override;
    void setColor(int red, int green, int blue);
    void setColor(GridColor gridcolor);

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

    // grid
    QVector<QVector3D> m_vertices;
    float m_width;
    float m_gap;
    Color m_color;
    Axis m_axisX;
    Axis m_axisZ;

    void bufferInit() override;
    void gemGridLine();

};

#endif // GRIDLINE_H
