#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    auto* wavefrontObj = new WavefrontOBJ();

    QVector<QVector3D> normals;
    QVector<QVector2D> uvs;
    QVector<GLushort> uvIndices;
    QVector<GLushort> normalIndices;


    //wavefrontObj->load(":/cube.obj", m_vertices, m_indices, uvs, uvIndices, normals, normalIndices);

    m_vertices << QVector3D(-1, 0, 1) << QVector3D(1, 0, 1) << QVector3D(-1, 0, -1) << QVector3D(1, 0, -1);
    m_indices << 2 << 3 << 1 << 2 << 1 << 0;

    m_cameraAngle = QVector2D(20.0, -20.0);
    m_cameraDistance = 2.5f;

    qDebug() << m_vertices.toList();
    qDebug() << m_indices.toList();
    qDebug() << m_vertices.size();
    qDebug() << m_indices.size();
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();


    /* enabled */
    glEnable(GL_DEPTH_TEST);    // Enable depth buffer
    //glEnable(GL_CULL_FACE); // Enable back face culling

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

    m_index = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_index.create();
    m_index.bind();
    //m_index.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_index.allocate(m_indices.constData(), m_indices.size() * static_cast<int>(sizeof(GLushort)));
    m_index.release();

    // 色情報をVBOに転送する
    m_color.create();
    m_color.bind();
    m_color.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_color.allocate(m_colors.constData(), m_colors.size() * static_cast<int>(sizeof(QVector3D)));
    m_color.release();

    // VBOをVAOに紐づける
    m_shaderProgram->bind();

    m_vertex.bind();
    m_index.bind();
    m_shaderProgram->enableAttributeArray("qt_Vertex");
    m_shaderProgram->setAttributeBuffer("qt_Vertex", GL_FLOAT, 0, 3);





    m_color.bind();
    m_shaderProgram->enableAttributeArray("qt_Color");
    m_shaderProgram->setAttributeBuffer("qt_Color", GL_FLOAT, 0, 3);
    m_color.release();

    m_shaderProgram->enableAttributeArray("qt_ModelViewProjectionMatrix");


    m_shaderProgram->release();
    m_vao.release();
m_index.release();
m_vertex.release();
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

    /* Draw */
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("qt_ModelViewProjectionMatrix", m_projection * viewMatrix * modelMatrix);

    m_vao.bind();
    glDrawElements(GL_TRIANGLE_STRIP, m_vertices.size(), GL_UNSIGNED_SHORT, nullptr);
    m_vao.release();

    m_shaderProgram->release();
}


void GLWidget::resizeGL(int w, int h)
{
    float aspect = float(w) / float(h ? h : 1);
    const float zNear = 0.001f, zFar = 1000, fov = 60.0;

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
