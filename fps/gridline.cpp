#include "gridline.h"

GridLine::GridLine()
{
    initializeOpenGLFunctions();

    m_width = 500;
    m_gap = 1.0f;

    gemGridLine();
    setColor(72, 72, 72);
    m_color.axisX = QVector3D(167 / 255.0f,  68 / 255.0f,  83 / 255.0f);
    m_color.axisY = QVector3D(109 / 255.0f, 150 / 255.0f,  45 / 255.0f);
    m_color.axisZ = QVector3D( 62 / 255.0f,  95 / 255.0f, 139 / 255.0f);
}

GridLine::~GridLine()
{
    delete m_shaderProgram;
    m_vao.destroy();
    m_vertex.destroy();
}

void GridLine::shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile)
{
    m_shaderProgram = new QOpenGLShaderProgram();

    // Compile shader
    Q_ASSERT(m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile));
    Q_ASSERT(m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile));

    // Link shader pipeline
    Q_ASSERT(m_shaderProgram->link());

    // Bind shader pipeline for use
    Q_ASSERT(m_shaderProgram->bind());

    m_shaderProgram->release();
}

void GridLine::bufferInit()
{
    /* VAO */
    m_vao.create();
    m_vao.bind();

    // 頂点情報をVBOに転送する
    m_vertex = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vertex.create();
    m_vertex.bind();
    m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertex.allocate(m_vertices.constData(), m_vertices.size() * static_cast<int>(sizeof(QVector3D)));
    m_vertex.release();

    // VBOをVAOに紐づける
    m_shaderProgram->bind();

    m_vertex.bind();
    m_shaderProgram->enableAttributeArray("VertexPosition");
    m_shaderProgram->setAttributeBuffer("VertexPosition", GL_FLOAT, 0, 3);
    m_vertex.release();

    m_shaderProgram->enableAttributeArray("VertexColor");
    m_shaderProgram->enableAttributeArray("MVP");

    m_shaderProgram->release();
    m_vao.release();
}

void GridLine::gemGridLine()
{
    float start = -m_width;
    float end = m_width;

    // Axis X
    m_axisX.first = m_vertices.size();
    m_vertices.append(QVector3D(start, 0.0f, 0.0f));
    m_vertices.append(QVector3D(end, 0.0f, 0.0f));
    m_axisX.count = m_vertices.size();

    // Axis Z
    m_axisZ.first = m_vertices.size();
    m_vertices.append(QVector3D(0.0f, 0.0f, start));
    m_vertices.append(QVector3D(0.0f, 0.0f, end));
    m_axisZ.count = m_vertices.size();

    qDebug() << m_axisZ.first;
    qDebug() << m_axisZ.count;

    // X軸のライン
    for (int i = 1; i < m_width; i++) {
        float z = i * m_gap;
        m_vertices.append(QVector3D(start, 0.0f, z));
        m_vertices.append(QVector3D(end, 0.0f, z));
        m_vertices.append(QVector3D(start, 0.0f, -z));
        m_vertices.append(QVector3D(end, 0.0f, -z));
    }

    // Z軸のライン
    for (int i = 1; i < m_width; i++) {
        float x = i * m_gap;
        m_vertices.append(QVector3D(x, 0.0f, start));
        m_vertices.append(QVector3D(x, 0.0f, end));
        m_vertices.append(QVector3D(-x, 0.0f, start));
        m_vertices.append(QVector3D(-x, 0.0f, end));
    }
}

void GridLine::bind(const QString &vertexShader, const QString &fragmentShader)
{
    shaderInit(vertexShader, fragmentShader);
    bufferInit();
}

void GridLine::draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix)
{
    // Model Matrix
    QMatrix4x4 modelMatrix;

    // set uniform
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("VertexColor", m_color.grid);
    m_shaderProgram->setUniformValue("MVP", projectionMatrix * viewMatrix * modelMatrix);


    // Draw
    glLineWidth(1.5f);

    m_vao.bind();
    glDrawArrays(GL_LINES, 4, m_vertices.size());

    m_shaderProgram->setUniformValue("VertexColor", m_color.axisX);
    glDrawArrays(GL_LINES, m_axisX.first, m_axisX.count);

    m_shaderProgram->setUniformValue("VertexColor", m_color.axisZ);
    glDrawArrays(GL_LINES, m_axisZ.first, m_axisZ.count);

    m_vao.release();
    m_shaderProgram->release();
}

void GridLine::setColor(int red, int green, int blue)
{
    m_color.grid = QVector3D(red / 255.0f, green / 255.0f, blue / 255.0f);
}
