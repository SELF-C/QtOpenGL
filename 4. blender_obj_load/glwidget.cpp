#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    // メッシュの読み込み
    QVector<Triangle3D> triangles;
    QStringList comments;
    WavefrontOBJ().parser(":/cube.obj", comments, triangles);

    m_vertices.clear();
    m_normals.clear();

    for(int i = 0; i < triangles.count(); i++)
    {
        m_vertices.append(triangles.at(i).p1);
        m_vertices.append(triangles.at(i).p2);
        m_vertices.append(triangles.at(i).p3);

        m_normals.append(triangles.at(i).p1Normal);
        m_normals.append(triangles.at(i).p2Normal);
        m_normals.append(triangles.at(i).p3Normal);
    }

    // カメラ設定
    m_cameraAngle = QVector2D(20.0, -20.0);
    m_cameraDistance = 2.5f;

    // メッシュデータの設定
    m_angle = 0;
    m_scale = 1;
    m_translate = 0;
    parent->installEventFilter(this);
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    /* enabled */
    glEnable(GL_DEPTH_TEST);    // Enable depth buffer
    glEnable(GL_CULL_FACE); // Enable back face culling

    //const float pos[] = {100, 100, 512, 0};
    //glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_shaderProgram = new QOpenGLShaderProgram();

    // Compile vertex shader
    if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert"))
        close();

    // Compile fragment shader
    if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag"))
        close();

    // Link shader pipeline
    if (!m_shaderProgram->link())
        close();

    // Bind shader pipeline for use
    if (!m_shaderProgram->bind())
        close();

    m_shaderProgram->release();

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

    // 色情報をVBOに転送する
//    m_color.create();
//    m_color.bind();
//    m_color.setUsagePattern(QOpenGLBuffer::StaticDraw);
//    m_color.allocate(m_colors.constData(), m_colors.size() * static_cast<int>(sizeof(QVector3D)));
//    m_color.release();

    m_normal.create();
    m_normal.bind();
    m_normal.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_normal.allocate(m_normals.constData(), m_normals.size() * static_cast<int>(sizeof(QVector3D)));
    m_normal.release();

    // VBOをVAOに紐づける
    m_shaderProgram->bind();

    m_vertex.bind();
    m_shaderProgram->enableAttributeArray("qt_Vertex");
    m_shaderProgram->setAttributeBuffer("qt_Vertex", GL_FLOAT, 0, 3);
    m_vertex.release();

//    m_color.bind();
//    m_shaderProgram->enableAttributeArray("qt_Color");
//    m_shaderProgram->setAttributeBuffer("qt_Color", GL_FLOAT, 0, 3);
//    m_color.release();

    m_normal.bind();
    m_shaderProgram->enableAttributeArray("qt_Normal");
    m_shaderProgram->setAttributeBuffer("qt_Normal", GL_FLOAT, 0, 3);
    m_normal.release();

    m_shaderProgram->enableAttributeArray("qt_LightPosition");
    m_shaderProgram->enableAttributeArray("qt_Kd");
    m_shaderProgram->enableAttributeArray("qt_Ld");
    m_shaderProgram->enableAttributeArray("qt_ModelViewMatrix");
    m_shaderProgram->enableAttributeArray("qt_NormalMatrix");
    m_shaderProgram->enableAttributeArray("qt_ModelViewProjectionMatrix");

    m_shaderProgram->release();
    m_vao.release();

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


    /* Model Matrix */
    QMatrix4x4 modelMatrix;

    QQuaternion rotation =
            QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), 0) *
            QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), 0) *
            QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), m_angle);


    modelMatrix.translate(QVector3D(0.0f, 0.0f, m_translate));
    modelMatrix.rotate(rotation);
    modelMatrix.scale(m_scale);

    /* Draw */
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("qt_LightPosition", QVector4D(-25,25,25,1));
    m_shaderProgram->setUniformValue("qt_Kd", QVector3D(1,1,1));
    m_shaderProgram->setUniformValue("qt_Ld", QVector3D(1,1,1));
    m_shaderProgram->setUniformValue("qt_ModelViewMatrix", viewMatrix * modelMatrix);
    m_shaderProgram->setUniformValue("qt_NormalMatrix", modelMatrix.normalMatrix());
    m_shaderProgram->setUniformValue("qt_ModelViewProjectionMatrix", m_projection * viewMatrix * modelMatrix);

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    m_vao.release();

    m_shaderProgram->release();
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
    switch(event->type())
    {
        case QEvent::KeyPress:
            if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_A)
            {
                m_angle += 5;
            }
            if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_D)
            {
                m_angle -= 5;
            }
            if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_W)
            {
                m_translate += 0.1f;
            }
            if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_S)
            {
                m_translate -= 0.1f;
            }
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
