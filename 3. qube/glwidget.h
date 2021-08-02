#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);

protected:

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);



private:
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_colors;

    QMatrix4x4 m_projection;

    QOpenGLShaderProgram* m_program;
    QOpenGLVertexArrayObject m_object;
    QOpenGLBuffer m_vertex;

    /* Camera */
    QVector2D m_cameraAngle;
    float m_cameraDistance;

    int m_angle;
    float m_scale;
    float m_translate;

    QPoint m_mousePosition;

};

#endif // GLWIDGET_H
