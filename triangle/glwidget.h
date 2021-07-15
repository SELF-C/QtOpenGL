#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>

class Vertex{
public:
    Vertex();
    Vertex(const QVector3D &position): m_position(position){}
    static int positionOffset(){ return offsetof(Vertex, m_position); }
    static int stride(){ return sizeof(Vertex); }
private:
    QVector3D m_position;
};

static const Vertex m_vertices[] = {
    Vertex( QVector3D( 0.00f,  0.5f, 1.0f)),
    Vertex( QVector3D( 0.5f, -0.5f, 1.0f)),
    Vertex( QVector3D(-0.5f, -0.5f, 1.0f)),
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);

protected:

    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:

    QMatrix4x4 m_projection;

    QOpenGLShaderProgram* m_program;
    QOpenGLVertexArrayObject m_object;
    QOpenGLBuffer m_vertex;
    int m_vertexLocation;

};

#endif // GLWIDGET_H
