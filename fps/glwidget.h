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
#include <QtMath>
#include <QLabel>
#include "wavefrontobj.h"
#include "model.h"
#include "gridline.h"
#include "fpsmanager.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void updateGL();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    struct DebugInfo
    {
        QLabel* fps;
        QLabel* translation;
        QLabel* rotation;
        QLabel* scale;
        QLabel* mouse;
    };


    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;

    /* Camera */
    QVector2D m_cameraAngle;
    float m_cameraDistance;

    QPoint m_mousePosition;

    GridLine* m_gridline;
    Model* m_model;
    QVector3D m_angle;
    QVector3D m_translation;
    float m_scale;

    FpsManager* m_fps;

    // Debug
    DebugInfo m_info;

    void debugInfoInit();
    void debugInfoUpdate();
};

#endif // GLWIDGET_H
