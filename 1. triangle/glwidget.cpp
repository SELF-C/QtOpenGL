#include "glwidget.h"



GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m_vertices << QVector3D( 0.0f,  0.5f, 1.0f)
               << QVector3D( 0.5f, -0.5f, 1.0f)
               << QVector3D(-0.5f, -0.5f, 1.0f);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_program = new QOpenGLShaderProgram();

    // Compile vertex shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert"))
        close();

    // Compile fragment shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag"))
        close();

    // Link shader pipeline
    if (!m_program->link())
        close();

    // Bind shader pipeline for use
    if (!m_program->bind())
        close();


    // Transfer vertex data to VBO
    m_vertex.create();
    m_vertex.bind();
    m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertex.allocate(m_vertices.constData(), m_vertices.size() * sizeof(QVector3D));

    m_object.create();
    m_object.bind();

    m_program->enableAttributeArray("qt_Vertex");
    m_program->setAttributeBuffer("qt_Vertex", GL_FLOAT, 0, 3);

    m_object.release();
    m_vertex.release();
    m_program->release();
}



void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    auto vertexColor = QVector3D(1.0f, 1.0f, 1.0f);
    m_program->bind();
    m_object.bind();

    int colorLocation = m_program->uniformLocation("qt_Color");
    m_program->setUniformValue(colorLocation, vertexColor);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertices.size());

    m_object.release();
    m_program->release();
}
