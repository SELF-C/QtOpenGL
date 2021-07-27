#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    // カメラ設定
    m_cameraAngle = QVector2D(20.0, -20.0);
    m_cameraDistance = 2.5f;

    m_translation = QVector3D(0.0f, 0.0f, 0.0f);
    m_angle = QVector3D(0.0f, 0.0f, 0.0f);
    m_scale = 1;

    // eventFilterをGLWidgetに反映させる
    parent->installEventFilter(this);
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    /* enabled */
    glEnable(GL_DEPTH_TEST);    // Enable depth buffer
    glEnable(GL_CULL_FACE); // Enable back face culling

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_mesh = new Mesh();
    if (!m_mesh->load(":/Monkey.obj"))
        close();
    m_mesh->bind(":/shader.vert", ":/shader.frag");
}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* View Matrix */
    QMatrix4x4 camera;  // カメラを原点に沿って回転させる
    camera.rotate(m_cameraAngle.x(), QVector3D(0.0f, 1.0f, 0.0f));
    camera.rotate(m_cameraAngle.y(), QVector3D(1.0f, 0.0f, 0.0f));

    auto eye = camera * QVector3D(0.0f, 0.0f, m_cameraDistance);  // 仮想3Dカメラが配置されているポイント
    auto center = camera * QVector3D(0.0f, 0.0f, 0.0f);              // カメラが注視するポイント（シーンの中心）
    auto up = camera * QVector3D(0.0f, 1.0f, 0.0f);                  // 3Dワールドの上方向を定義

    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(eye, center, up);


    // Mesh draw
    m_mesh->setTranslation(m_translation.x(), m_translation.y(), m_translation.z());
    m_mesh->setRotation(m_angle.x(), m_angle.y(), m_angle.z());
    m_mesh->setScale(m_scale);
    m_mesh->draw(m_projection, viewMatrix);

}


void GLWidget::resizeGL(int w, int h)
{
    float aspect = float(w) / float(h ? h : 1);
    const float zNear = 0.1f, zFar = 1000, fov = 60.0;

    m_projection.setToIdentity();
    m_projection.perspective(fov, aspect, zNear, zFar);
    glViewport(0, 0, w, h);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePosition = event->pos();
    event->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    /* カメラの回転量をマウスの移動量から設定 */
    int deltaX = event->x() - m_mousePosition.x();
    int deltaY = event->y() - m_mousePosition.y();

    if (event->buttons() & Qt::RightButton) {
        /* カメラのX方向への回転角度制限 */
        m_cameraAngle.setX(m_cameraAngle.x() - deltaX);
        if (m_cameraAngle.x() < 0)
            m_cameraAngle.setX(m_cameraAngle.x() + 360);

        if (m_cameraAngle.x() >= 360)
            m_cameraAngle.setX(m_cameraAngle.x() - 360);

        /* カメラのY方向への回転角度制限 */
        m_cameraAngle.setY(m_cameraAngle.y() - deltaY);
        if (m_cameraAngle.y() < -90)
            m_cameraAngle.setY(-90);

        if (m_cameraAngle.y() > 90)
            m_cameraAngle.setY(90);

        update();
    }

    m_mousePosition = event->pos();

    event->accept();

}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    /* マウスホイールでカメラの奥行きの移動量を設定 */
    float delta = event->delta();
    if (event->orientation() == Qt::Vertical) {
        if (delta < 0) {
            m_cameraDistance *= 1.1f;
        }
        else if (delta > 0)
        {
            m_cameraDistance *= 0.9f;
        }
        update();
    }
    event->accept();

}

bool GLWidget::eventFilter(QObject *obj, QEvent *event)
{
    (void) obj;

    float deltaY = 5.0f;

    switch(event->type()){
    case QEvent::KeyPress:
        // 回転操作
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_A)
        {
            m_angle.setY(m_angle.y() + deltaY);
            if (m_angle.y() >= 360)
                m_angle.setY(m_angle.y() - 360);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_D)
        {
            m_angle.setY(m_angle.y() - deltaY);

            if (m_angle.y() < 0)
                m_angle.setY(m_angle.y() + 360);
        }

        // 移動操作
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_W)
        {
            m_translation.setZ( m_translation.z() + 0.1f);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_S)
        {
            m_translation.setZ( m_translation.z() - 0.1f);
        }

        // 拡大操作
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Q)
        {
            m_scale -= 0.1f;
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_E)
        {
            m_scale += 0.1f;
        }
        update();
        break;
    default:
        break;
    }

    return false;
}
