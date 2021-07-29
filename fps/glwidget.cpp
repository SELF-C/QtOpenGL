#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{

    // デバッグビルド時のみデバッグ情報の初期化をする
//#ifdef QT_DEBUG
    //debugInfoInit();
    //qDebug() << "QT_DEBUG";
//#endif


    // FPS
    //m_fps = new FpsManager();

    // Camera
    m_cameraAngle = QVector2D(20.0, -20.0);
    m_cameraDistance = 2.5f;

    // Model Transform
    m_translation = QVector3D(0.0f, 0.0f, 0.0f);
    m_angle = QVector3D(0.0f, 0.0f, 0.0f);
    m_scale = 1;

    // eventFilterをGLWidgetに反映させる
    parent->installEventFilter(this);
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // enabled
    glEnable(GL_DEPTH_TEST);        // Zバッファ
    glEnable(GL_CULL_FACE);         // カリング
    glEnable(GL_LINE_SMOOTH);       // アンチエイリアス(線)
    glEnable(GL_POLYGON_SMOOTH );   // アンチエイリアス(ポリゴン)
    glEnable(GL_BLEND);             // ブレンディング
    glEnable(GL_MULTISAMPLE);       // マルチサンプリング

    // background color
    QColor c(60, 60, 60);
    glClearColor(c.red() / 255.0f, c.green() / 255.0f, c.blue() / 255.0f, 1.0f);

    // init gridline
    m_gridline = new GridLine();
    m_gridline->bind(":/gridline.vert", ":/gridline.frag");

    // init model
    m_model = new Model();
    if (!m_model->load(":/test_model.obj"))
        close();
    m_model->bind(":/shader.vert", ":/shader.frag");
}


void GLWidget::paintGL()
{
    updateGL();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw Gridline
    m_gridline->draw(m_projectionMatrix, m_viewMatrix);

    // Draw Model
    m_model->draw(m_projectionMatrix, m_viewMatrix);
}


void GLWidget::resizeGL(int w, int h)
{
    float aspect = float(w) / float(h ? h : 1);
    const float zNear = 0.1f, zFar = 1000, fov = 60.0;

    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(fov, aspect, zNear, zFar);
    glViewport(0, 0, w, h);
}

void GLWidget::updateGL()
{
    //m_fps->frameRateCalculator();
//#ifdef _DEBUG
    //debugInfoUpdate();
//#endif

    //　ビューの更新処理
    QMatrix4x4 camera;  // カメラを原点に沿って回転させる
    camera.rotate(m_cameraAngle.x(), QVector3D(0.0f, 1.0f, 0.0f));
    camera.rotate(m_cameraAngle.y(), QVector3D(1.0f, 0.0f, 0.0f));

    auto eye = camera * QVector3D(0.0f, 0.0f, m_cameraDistance);  // 仮想3Dカメラが配置されているポイント
    auto center = camera * QVector3D(0.0f, 0.0f, 0.0f);           // カメラが注視するポイント（シーンの中心）
    auto up = camera * QVector3D(0.0f, 1.0f, 0.0f);               // 3Dワールドの上方向を定義

    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(eye, center, up);

    // モデルの更新処理
    m_model->setTranslation(m_translation.x(), m_translation.y(), m_translation.z());
    m_model->setRotation(m_angle.x(), m_angle.y(), m_angle.z());
    m_model->setScale(m_scale);
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
    }
    m_mousePosition = event->pos();

    update();
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
    (void) obj; // 未使用引数の警告をださない

    const float deltaY = 5.0f;  // 回転量
    const float acc = 0.1f; // 移動量

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
            qreal theta = static_cast<qreal>(m_angle.y()) / 180 * M_PI;
            m_translation.setX( m_translation.x() + static_cast<float>(qSin(theta)) * acc);
            m_translation.setZ( m_translation.z() + static_cast<float>(qCos(theta)) * acc);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_S)
        {
            qreal theta = static_cast<qreal>(m_angle.y()) / 180 * M_PI;
            m_translation.setX( m_translation.x() - static_cast<float>(qSin(theta)) * acc);
            m_translation.setZ( m_translation.z() - static_cast<float>(qCos(theta)) * acc);
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

void GLWidget::debugInfoInit()
{
    m_info.fps = new QLabel("FPS: ", this);
    m_info.translation = new QLabel("Translation: ", this);
    m_info.rotation = new QLabel("Rotation: ", this);
    m_info.scale = new QLabel("Scale: ", this);
    m_info.mouse = new QLabel("Mouse: ", this);

    m_info.fps->setStyleSheet("QLabel { color : white; }");
    auto stylesheet = m_info.fps->styleSheet();
    m_info.translation->setStyleSheet(stylesheet);
    m_info.rotation->setStyleSheet(stylesheet);
    m_info.scale->setStyleSheet(stylesheet);
    m_info.mouse->setStyleSheet(stylesheet);

    int w = 400;
    int h = m_info.fps->height();
    m_info.fps->setGeometry(10, h * 0, w, h);
    m_info.translation->setGeometry(10, h * 1, w, h);
    m_info.rotation->setGeometry(10, h * 2, w, h);
    m_info.scale->setGeometry(10, h * 3, w, h);
    m_info.mouse->setGeometry(10, h * 4, w, h);
}

void GLWidget::debugInfoUpdate()
{
    float scale = (m_scale == 0.0f) ? 0.0f : m_scale * 100.0f;

    m_info.fps->setText(QString("FPS %1").arg(m_fps->getFps()));
    m_info.translation->setText(QString("Translation X:%1, Y:%2, Z:%3").arg(m_translation.x()).arg(m_translation.y()).arg(m_translation.z()));
    m_info.rotation->setText(QString("Rotation X:%1, Y:%2, Z:%3").arg(m_angle.x()).arg(m_angle.y()).arg(m_angle.z()));
    m_info.scale->setText(QString("Scale %1%").arg(scale));
    m_info.mouse->setText(QString("Mouse X:%1, Y:%2, Z:%3").arg(m_cameraAngle.x()).arg(m_cameraAngle.y()).arg(m_cameraDistance));

    update();
}
