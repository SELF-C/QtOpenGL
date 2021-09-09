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
#include <QMenuBar>
#include <QApplication>
#include <QPushButton>
#include <QtGlobal>
#include <QTime>
#include "model.h"
#include "gridline.h"
#include "fpsmanager.h"
#include "gldebug.h"

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
    struct Transform
    {
        QVector3D angle;
        QVector3D translation;
        float scale = 1;
    };

    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;

    /* Camera */
    QVector2D m_cameraAngle;
    float m_cameraDistance;

    QPoint m_mousePosition;

    GridLine* m_gridline;
    QVector<Model*> m_model;
    QVector<Model*> m_sphere;
    int m_activeModelIndex = 0;

    QVector<Transform> m_transform;

    FpsManager* m_fps;

    // Debug
    GLDebug* m_gldebug;

    QPushButton* m_button;
};

#endif // GLWIDGET_H
