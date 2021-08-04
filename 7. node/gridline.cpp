﻿#include "gridline.h"

GridLine::GridLine()
{
    initialize();

    m_width = 500;
    m_gap = 1.0f;

    gemGridLine();
    setColor(GridColor::Blender);
}


void GridLine::bufferInit()
{
    // 頂点情報をVBOに転送する
    getVbo() = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    getVbo().create();
    getVbo().setUsagePattern(QOpenGLBuffer::StaticDraw);
    getVbo().bind();
    getVbo().allocate(getVertices().constData(), getVertices().size() * static_cast<int>(sizeof(VertexData)));
    getVbo().release();

    // シェーダーで使用する属性の設定
    getShaderProgram()->bind();
    getShaderProgram()->enableAttributeArray("VertexColor");
    getShaderProgram()->enableAttributeArray("MVP");
    getShaderProgram()->release();
}

void GridLine::gemGridLine()
{
    float start = -m_width;
    float end = m_width;

    QVector<VertexData> vertices;
    auto normal = QVector3D(0.0f, 0.0f, 0.0f);
    auto texCoord = QVector2D(0.0f, 0.0f);

    // Axis X
    m_axisX.first = vertices.size();
    vertices.append(VertexData{ QVector3D(start, 0.0f, 0.0f), normal, texCoord });
    vertices.append(VertexData{ QVector3D(end, 0.0f, 0.0f), normal, texCoord });
    m_axisX.count = vertices.size();

    // Axis Z
    m_axisZ.first = vertices.size();
    vertices.append(VertexData{ QVector3D(0.0f, 0.0f, start), normal, texCoord });
    vertices.append(VertexData{ QVector3D(0.0f, 0.0f, end), normal, texCoord });
    m_axisZ.count = vertices.size();

    // X軸のライン
    for (int i = 1; i < m_width; i++) {
        float z = i * m_gap;
        vertices.append(VertexData{ QVector3D(start, 0.0f, z), normal, texCoord });
        vertices.append(VertexData{ QVector3D(end, 0.0f, z), normal, texCoord });
        vertices.append(VertexData{ QVector3D(start, 0.0f, -z), normal, texCoord });
        vertices.append(VertexData{ QVector3D(end, 0.0f, -z), normal, texCoord });
    }

    // Z軸のライン
    for (int i = 1; i < m_width; i++) {
        float x = i * m_gap;
        vertices.append(VertexData{ QVector3D(x, 0.0f, start), normal, texCoord });
        vertices.append(VertexData{ QVector3D(x, 0.0f, end), normal, texCoord });
        vertices.append(VertexData{ QVector3D(-x, 0.0f, start), normal, texCoord });
        vertices.append(VertexData{ QVector3D(-x, 0.0f, end), normal, texCoord });
    }

    setVertices(vertices);
}

void GridLine::draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix)
{
    // Model Matrix
    QMatrix4x4 modelMatrix;

    // set uniform
    getShaderProgram()->bind();
    getShaderProgram()->setUniformValue("VertexColor", m_color.grid);
    getShaderProgram()->setUniformValue("MVP", projectionMatrix * viewMatrix * modelMatrix);

    glLineWidth(1.5f);

    // Draw Gridline
    getVbo().bind();

    getShaderProgram()->enableAttributeArray("VertexPosition");
    getShaderProgram()->setAttributeBuffer("VertexPosition", GL_FLOAT, 0, 3, sizeof(VertexData));

    glDrawArrays(GL_LINES, 4, getVertices().size());

    // Axis X
    getShaderProgram()->setUniformValue("VertexColor", m_color.axisX);
    glDrawArrays(GL_LINES, m_axisX.first, m_axisX.count);

    // Axis Y
    getShaderProgram()->setUniformValue("VertexColor", m_color.axisZ);
    glDrawArrays(GL_LINES, m_axisZ.first, m_axisZ.count);

    getVbo().release();
    getShaderProgram()->release();
}

void GridLine::setColor(int red, int green, int blue)
{
    m_color.grid = QVector3D(red / 255.0f, green / 255.0f, blue / 255.0f);
}

void GridLine::setColor(GridColor gridcolor)
{
    switch (gridcolor) {
    case GridColor::Blender:
        m_color.grid  = QVector3D( 72 / 255.0f,  72 / 255.0f,  72 / 255.0f);
        m_color.axisX = QVector3D(167 / 255.0f,  68 / 255.0f,  83 / 255.0f);
        m_color.axisY = QVector3D(109 / 255.0f, 150 / 255.0f,  45 / 255.0f);
        m_color.axisZ = QVector3D( 62 / 255.0f,  95 / 255.0f, 139 / 255.0f);
        break;
    case GridColor::Metasequoia:
        break;
    case GridColor::Unity:
        break;
    case GridColor::UnrealEngine:
        break;
    }
}
