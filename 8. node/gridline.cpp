#include "gridline.h"

GridLine::GridLine()
{
    initialize();

    m_width = 500;
    m_gap = 1.0f;

    setColor(GridColor::Blender);
    gemGridLine();
}

void GridLine::release()
{
    m_vbo.release();
    m_vbo.destroy();
    m_ibo.release();
    m_ibo.destroy();
}

void GridLine::gemGridLine()
{
    m_vertices.clear();

    float start = -m_width;
    float end = m_width;

    // Axis X
    m_vertices.append(VertexData{ QVector3D(start, 0.0f, 0.0f), m_color.axisX });
    m_vertices.append(VertexData{ QVector3D(end, 0.0f, 0.0f), m_color.axisX });

    // Axis Z
    m_vertices.append(VertexData{ QVector3D(0.0f, 0.0f, start), m_color.axisZ });
    m_vertices.append(VertexData{ QVector3D(0.0f, 0.0f, end), m_color.axisZ });

    // X軸のライン
    for (int i = 1; i < m_width; i++) {
        float z = i * m_gap;
        m_vertices.append(VertexData{ QVector3D(start, 0.0f,  z), m_color.grid });
        m_vertices.append(VertexData{ QVector3D(  end, 0.0f,  z), m_color.grid });
        m_vertices.append(VertexData{ QVector3D(start, 0.0f, -z), m_color.grid });
        m_vertices.append(VertexData{ QVector3D(  end, 0.0f, -z), m_color.grid });
    }

    // Z軸のライン
    for (int i = 1; i < m_width; i++) {
        float x = i * m_gap;
        m_vertices.append(VertexData{ QVector3D( x, 0.0f, start), m_color.grid });
        m_vertices.append(VertexData{ QVector3D( x, 0.0f, end), m_color.grid });
        m_vertices.append(VertexData{ QVector3D(-x, 0.0f, start), m_color.grid });
        m_vertices.append(VertexData{ QVector3D(-x, 0.0f, end), m_color.grid });
    }

    for (int i = 0; i < m_vertices.size(); i++) {
        m_indexes.append(static_cast<GLushort>(i));
    }
}

void GridLine::bufferInit()
{
    // 頂点バッファを生成
    m_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo.create();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.bind();
    m_vbo.allocate(m_vertices.constData(), m_vertices.size() * static_cast<int>(sizeof(VertexData)));
    m_vbo.release();

    // インデックスバッファを生成
    m_ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_ibo.create();
    m_ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ibo.bind();
    m_ibo.allocate(m_indexes.constData(), m_indexes.size() * static_cast<int>(sizeof(GLushort)));
    m_ibo.release();

    m_vertices.clear();

    // シェーダーで使用する属性の設定
    getShaderProgram()->bind();
    getShaderProgram()->enableAttributeArray("MVP");
    getShaderProgram()->release();
}




void GridLine::draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &parentWorldMatrix)
{
    QMatrix4x4 modelMatrix;

    // set uniform
    getShaderProgram()->bind();
    getShaderProgram()->setUniformValue("MVP", projectionMatrix * viewMatrix * modelMatrix);

    glLineWidth(1.5f);

    // Draw Gridline
    m_vbo.bind();
    m_ibo.bind();

    int offset = 0;
    getShaderProgram()->enableAttributeArray("VertexPosition");
    getShaderProgram()->setAttributeBuffer("VertexPosition", GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    getShaderProgram()->enableAttributeArray("VertexColor");
    getShaderProgram()->setAttributeBuffer("VertexColor", GL_FLOAT, offset, 3, sizeof(VertexData));

    glDrawElements(GL_LINES, m_indexes.size(), GL_UNSIGNED_SHORT, nullptr);

    m_ibo.release();
    m_vbo.release();
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
