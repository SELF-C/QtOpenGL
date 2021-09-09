#include "glwidget.h"
#include <QFileDialog>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setVersion(4,0);
    format.setDepthBufferSize(24);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    // eventFilterをGLWidgetに反映させる
    parent->installEventFilter(this);

    m_transform.append(Transform());
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

    // Camera
    m_cameraAngle = QVector2D(20.0, -20.0);
    m_cameraDistance = 2.5f;

    // Model Initialize
    m_model.clear();
    m_model.append(new Model());
    m_model.last()->load("C:/Users/nishioka_takuya/Desktop/untitled.stl");
    m_model.last()->bind(":/shader.vert", ":/shader.frag");

    m_model.append(new Model());
    m_model.last()->load(":/sphere.obj");
    m_model.last()->bind(":/shader.vert", ":/shader.frag");
    //m_model.last()->setEnabled(false);
    m_model.last()->setTranslation(QVector3D(0,0,-2.2f));
    m_model[m_model.size() - 2]->addChild(m_model.last());
    m_transform.append(Transform());
    m_transform.last().translation = QVector3D(0,0,-2.2f);

    m_model.append(new Model());
    m_model.last()->load(":/cube.obj");
    m_model.last()->bind(":/shader.vert", ":/shader.frag");
    m_model.last()->setTranslation(QVector3D(0,0,-2.5));
    m_model.last()->setRotation(QQuaternion::fromEulerAngles(QVector3D(0,0,0)));
    m_model[m_model.size() - 2]->addChild(m_model.last());
    m_transform.append(Transform());
    m_transform.last().translation = QVector3D(0,0,-2.5);


    // GLWidget MenuBar
    auto menuBar = new QMenuBar(this);
    menuBar->setFixedWidth(10000);

    auto file = new QMenu("File");
    menuBar->addMenu(file);

    auto open = new QAction("Open");
    file->addAction(open);
    connect(open, &QAction::triggered, this,
            [=](){
        auto filename = QFileDialog::getOpenFileName(this, "hoge", "", "Wavefront OBJ(*.obj);;All Files(*.*)");

        m_model.append(new Model());
        m_model.last()->load(filename);
        m_model.last()->bind(":/shader.vert", ":/shader.frag");
        m_transform.append(Transform());

        m_activeModelIndex = m_model.size()-1;
    });

    auto exit = new QAction("Exit");
    file->addAction(exit);
    connect(exit, &QAction::triggered, this, &QApplication::exit);

    m_button = new QPushButton("Add Sphere" ,this);
    m_button->setFixedWidth(m_button->width() + 20);
    m_button->move(this->width() - m_button->width(), 30);
    connect(m_button, &QPushButton::clicked, this,
            [=](){
        // 乱数のシード
        qsrand( static_cast<uint>(QTime::currentTime().msec()) );

        m_sphere.append(new Model());
        m_sphere.last()->load(":/sphere.obj");
        m_sphere.last()->bind(":/shader.vert", ":/shader.frag");
        m_sphere.last()->setTranslation(QVector3D(qrand() % 20 - 10, qrand() % 20 - 10, qrand() % 20 - 10));
        m_sphere.last()->setOpacity(0.3f);
        m_button->setText(QString("Add Sphere %1").arg(m_sphere.size()));
    });

    // FPS
    m_fps = new FpsManager();

    // デバッグビルド時のみデバッグ情報の初期化をする
#ifdef QT_DEBUG
    m_gldebug = new GLDebug(this);

    // OpenGL Info
    qDebug() << "OpenGL Ver. : " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << "Shader Ver. : " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    qDebug() << "Vendor      : " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    qDebug() << "GPU         : " << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
#endif
}


void GLWidget::paintGL()
{
    updateGL();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw Gridline
    //m_gridline->draw(m_projectionMatrix, m_viewMatrix);


    // Draw Model
    m_model.first()->draw(m_projectionMatrix, m_viewMatrix);


    for (int i = 0; i < m_sphere.size(); i++) {
        m_sphere.at(i)->draw(m_projectionMatrix, m_viewMatrix);
    }
}


void GLWidget::resizeGL(int w, int h)
{
    float aspect = float(w) / float(h ? h : 1);
    const float zNear = 0.1f, zFar = 10000, fov = 60.0;

    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(fov, aspect, zNear, zFar);
    glViewport(0, 0, w, h);
}

void GLWidget::updateGL()
{
    // FPS制御
    m_fps->frameRateCalculator();
    m_fps->fixedFrameRate(60);

    //　ビューの更新処理
    QMatrix4x4 camera;  // カメラを原点に沿って回転させる
    camera.rotate(m_cameraAngle.x(), QVector3D(0.0f, 1.0f, 0.0f));
    camera.rotate(m_cameraAngle.y(), QVector3D(1.0f, 0.0f, 0.0f));

    auto eye = camera * QVector3D(0.0f, 0.0f, m_cameraDistance);  // 仮想3Dカメラが配置されているポイント
    auto center = camera * QVector3D(0.0f, 0.0f, 0.0f);           // カメラが注視するポイント（シーンの中心）
    auto up = camera * QVector3D(0.0f, 1.0f, 0.0f);               // 3Dワールドの上方向を定義

    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(eye, center, up);

    m_gridline->update();
    m_model.first()->update();

    // モデルの更新処理
    float pitch = m_transform[m_activeModelIndex].angle.x();
    float yaw = m_transform[m_activeModelIndex].angle.y();
    float roll = m_transform[m_activeModelIndex].angle.z();
    m_model[m_activeModelIndex]->setRotation(QQuaternion::fromEulerAngles(pitch, yaw, roll));
    m_model[m_activeModelIndex]->setTranslation(m_transform[m_activeModelIndex].translation);
    m_model[m_activeModelIndex]->setScale(m_transform[m_activeModelIndex].scale);


#ifdef QT_DEBUG
    m_gldebug->update(
                m_fps->getFps(),
                m_activeModelIndex,
                m_transform.at(m_activeModelIndex).translation,
                m_transform.at(m_activeModelIndex).angle,
                m_transform.at(m_activeModelIndex).scale,
                QVector3D(m_cameraAngle, m_cameraDistance));
#endif

    this->update();
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

    // マウスの右クリックドラッグでカメラ回転
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
    }
    event->accept();
}

bool GLWidget::eventFilter(QObject *obj, QEvent *event)
{
    (void) obj; // 未使用引数の警告をださない

    const float delta = 5.0f;  // 回転量
    const float acc = 0.1f; // 移動量


    QVector3D angle = m_transform.at(m_activeModelIndex).angle;
    QVector3D translation = m_transform.at(m_activeModelIndex).translation;
    float scale = m_transform.at(m_activeModelIndex).scale;

    switch(event->type()){
    case QEvent::KeyPress:
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Tab)
        {
            if (m_activeModelIndex >= m_model.size() - 1)
            {
                m_activeModelIndex = 0;
            }
            else
            {
                m_activeModelIndex++;
            }
            angle = m_transform.at(m_activeModelIndex).angle;
            translation = m_transform.at(m_activeModelIndex).translation;
            scale = m_transform.at(m_activeModelIndex).scale;
        }

        // 回転操作
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_A)
        {
            angle.setY(angle.y() + delta);
            if (angle.y() >= 360)
                angle.setY(angle.y() - 360);

        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_D)
        {
            angle.setY(angle.y() - delta);

            if (angle.y() < 0)
                angle.setY(angle.y() + 360);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_W)
        {
            angle.setX(angle.x() + delta);

            if (angle.x() >= 360)
                angle.setX(angle.x() - 360);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_S)
        {
            angle.setX(angle.x() - delta);

            if (angle.x() < 0)
                angle.setX(angle.x() + 360);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Q)
        {
            angle.setZ(angle.z() + delta);

            if (angle.z() >= 360)
                angle.setZ(angle.z() - 360);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_E)
        {
            angle.setZ(angle.z() - delta);

            if (angle.z() < 0)
                angle.setZ(angle.z() + 360);
        }
        m_transform[m_activeModelIndex].angle = angle;

        // 移動操作
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Up)
        {
            qreal theta = static_cast<qreal>(angle.y()) / 180 * M_PI;
            translation.setX( translation.x() + static_cast<float>(qSin(theta)) * acc);
            translation.setZ( translation.z() + static_cast<float>(qCos(theta)) * acc);
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Down)
        {
            qreal theta = static_cast<qreal>(angle.y()) / 180 * M_PI;
            translation.setX( translation.x() - static_cast<float>(qSin(theta)) * acc);
            translation.setZ( translation.z() - static_cast<float>(qCos(theta)) * acc);
        }
        m_transform[m_activeModelIndex].translation = translation;

        // 拡大操作
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Z)
        {
            scale -= 0.1f;
        }
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_C)
        {
            scale += 0.1f;
        }
        m_transform[m_activeModelIndex].scale = scale;
        break;

    default:
        break;
    }

    return false;
}


