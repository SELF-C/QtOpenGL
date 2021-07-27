#include "mesh.h"
#include "QDebug"
Mesh::Mesh()
{
    initializeOpenGLFunctions();

    // メッシュデータの設定
    setRotation(0.0f, 0.0f, 0.0f);
    setTranslation(0.0f, 0.0f, 0.0f);
    setScale(1.0f);

    setLightPostion(-25.0f, 25.0f, 25.0f);
    setKd(1.0f, 1.0f, 1.0f);
    setLd(1.0f, 1.0f, 1.0f);
}

bool Mesh::load(const QString &filename)
{
    // メッシュの読み込み
    QVector<Triangle3D> triangles;
    QStringList comments;
    if (!WavefrontOBJ().parser(filename, comments, triangles))
        return false;

    m_vertices.clear();
    m_normals.clear();
    m_comments.clear();

    m_comments = comments;

    for(int i = 0; i < triangles.count(); i++)
    {
        m_vertices.append(triangles.at(i).p1);
        m_vertices.append(triangles.at(i).p2);
        m_vertices.append(triangles.at(i).p3);

        m_normals.append(triangles.at(i).p1Normal);
        m_normals.append(triangles.at(i).p2Normal);
        m_normals.append(triangles.at(i).p3Normal);
    }

    return true;
}

void Mesh::bind(const QString &vertexShader, const QString &fragmentShader)
{
    shaderInit(vertexShader, fragmentShader);
    bufferInit();
}

void Mesh::shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile)
{
    m_shaderProgram = new QOpenGLShaderProgram();

    // Compile shader
    Q_ASSERT(m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile));
    Q_ASSERT(m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile));

    // Link shader pipeline
    Q_ASSERT(m_shaderProgram->link());

    // Bind shader pipeline for use
    Q_ASSERT(m_shaderProgram->bind());

    m_shaderProgram->release();
}

void Mesh::bufferInit()
{
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

void Mesh::draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix)
{
    /* Model Matrix */
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_translation);
    modelMatrix.rotate(m_rotation);
    modelMatrix.scale(m_scale);

    /* Draw */
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("qt_LightPosition", m_lightPosition);
    m_shaderProgram->setUniformValue("qt_Kd", m_kd);
    m_shaderProgram->setUniformValue("qt_Ld", m_ld);
    m_shaderProgram->setUniformValue("qt_ModelViewMatrix", viewMatrix * modelMatrix);
    m_shaderProgram->setUniformValue("qt_NormalMatrix", modelMatrix.normalMatrix());
    m_shaderProgram->setUniformValue("qt_ModelViewProjectionMatrix", projectionMatrix * viewMatrix * modelMatrix);

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    m_vao.release();

    m_shaderProgram->release();
}

void Mesh::setRotation(float angleX, float angleY, float angleZ)
{
    m_rotation =
            QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angleZ) *
            QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), angleX) *
            QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angleY);
}

void Mesh::setTranslation(float x, float y, float z)
{
    m_translation = QVector3D(x, y, z);
}

void Mesh::setScale(float s)
{
    m_scale = s;
}

void Mesh::setLightPostion(float x, float y, float z)
{
    m_lightPosition = QVector4D(x, y, z, 1.0f);
}

void Mesh::setKd(float r, float g, float b)
{
    m_kd = QVector3D(r, g, b);
}

void Mesh::setLd(float r, float g, float b)
{
    m_ld = QVector3D(r, g, b);
}
