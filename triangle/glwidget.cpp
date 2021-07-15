#include "glwidget.h"



GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{

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
    m_vertex.allocate(m_vertices, sizeof(m_vertices));

    m_object.create();
    m_object.bind();

    m_vertexLocation  = m_program->attributeLocation("qt_Vertex");
    m_program->enableAttributeArray(m_vertexLocation);
    m_program->setAttributeBuffer(m_vertexLocation, GL_FLOAT, Vertex::positionOffset(), 3, Vertex::stride());

    m_object.release();
    m_vertex.release();
    m_program->release();
}



void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();
    m_object.bind();

    glDrawArrays(GL_TRIANGLES, 0, sizeof(m_vertices) / sizeof(m_vertices[0]));

    m_object.release();
    m_program->release();

}
