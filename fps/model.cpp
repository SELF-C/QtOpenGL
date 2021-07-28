#include "model.h"
#include "QDebug"

Model::Model()
{
    initializeOpenGLFunctions();

    // メッシュデータの初期設定
    setRotation(0.0f, 0.0f, 0.0f);
    setTranslation(0.0f, 0.0f, 0.0f);
    setScale(1.0f);

    // ライティングの初期設定
    setLight(QVector4D(-25.0f, 25.0f, 25.0f, 1.0f),
             QVector3D(0.3f, 0.3f, 0.3f),
             QVector3D(1.0f, 1.0f, 1.0f),
             QVector3D(1.0f, 1.0f, 1.0f));

    setMaterial(QVector3D(1.0f, 1.0f, 1.0f),
                QVector3D(0.8f, 0.8f, 0.8f),
                QVector3D(0.8f, 0.8f, 0.8f),
                100.0f);
}

Model::~Model()
{
    // And now release all OpenGL resources
    delete m_shaderProgram;
    m_vao.destroy();
    m_vertex.destroy();
    m_normal.destroy();
}

bool Model::load(const QString &filename)
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

void Model::bind(const QString &vertexShader, const QString &fragmentShader)
{
    shaderInit(vertexShader, fragmentShader);
    bufferInit();
}

void Model::shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile)
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

void Model::bufferInit()
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

    // 法線情報をVBOに転送する
    m_normal.create();
    m_normal.bind();
    m_normal.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_normal.allocate(m_normals.constData(), m_normals.size() * static_cast<int>(sizeof(QVector3D)));
    m_normal.release();

    // VBOをVAOに紐づける
    m_shaderProgram->bind();

    m_vertex.bind();
    m_shaderProgram->enableAttributeArray("VertexPosition");
    m_shaderProgram->setAttributeBuffer("VertexPosition", GL_FLOAT, 0, 3);
    m_vertex.release();

    m_normal.bind();
    m_shaderProgram->enableAttributeArray("VertexNormal");
    m_shaderProgram->setAttributeBuffer("VertexNormal", GL_FLOAT, 0, 3);
    m_normal.release();

    m_shaderProgram->enableAttributeArray("Light.Position");
    m_shaderProgram->enableAttributeArray("Light.La");
    m_shaderProgram->enableAttributeArray("Light.Ld");
    m_shaderProgram->enableAttributeArray("Light.Ls");
    m_shaderProgram->enableAttributeArray("Material.Ka");
    m_shaderProgram->enableAttributeArray("Material.Kd");
    m_shaderProgram->enableAttributeArray("Material.Ks");
    m_shaderProgram->enableAttributeArray("Material.Shininess");
    m_shaderProgram->enableAttributeArray("ModelViewMatrix");
    m_shaderProgram->enableAttributeArray("NormalMatrix");
    m_shaderProgram->enableAttributeArray("MVP");

    m_shaderProgram->release();
    m_vao.release();
}

void Model::draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix)
{
    /* Model Matrix */
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_translation);
    modelMatrix.rotate(m_rotation);
    modelMatrix.scale(m_scale);

    // set uniform
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("Light.Position", m_light.Position);
    m_shaderProgram->setUniformValue("Light.La", m_light.La);
    m_shaderProgram->setUniformValue("Light.Ld", m_light.Ld);
    m_shaderProgram->setUniformValue("Light.Ls", m_light.Ls);
    m_shaderProgram->setUniformValue("Material.Ka", m_material.Ka);
    m_shaderProgram->setUniformValue("Material.Kd", m_material.Kd);
    m_shaderProgram->setUniformValue("Material.Ks", m_material.Ks);
    m_shaderProgram->setUniformValue("Material.Shininess", m_material.Shininess);
    m_shaderProgram->setUniformValue("ModelViewMatrix", viewMatrix * modelMatrix);
    m_shaderProgram->setUniformValue("NormalMatrix", modelMatrix.normalMatrix());
    m_shaderProgram->setUniformValue("MVP", projectionMatrix * viewMatrix * modelMatrix);

    // Draw
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    m_vao.release();

    m_shaderProgram->release();
}

void Model::setRotation(float angleX, float angleY, float angleZ)
{
    m_rotation =
            QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angleZ) *
            QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), angleX) *
            QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angleY);
}

void Model::setTranslation(float x, float y, float z)
{
    m_translation = QVector3D(x, y, z);
}

void Model::setScale(float s)
{
    m_scale = s;
}

void Model::setLight(QVector4D position, QVector3D La, QVector3D Ld, QVector3D Ls)
{
    m_light.Position = position;
    m_light.La = La;
    m_light.Ld = Ld;
    m_light.Ls = Ls;
}

void Model::setMaterial(QVector3D Ka, QVector3D Kd, QVector3D Ks, float shininess)
{
    m_material.Ka = Ka;
    m_material.Kd = Kd;
    m_material.Ks = Ks;
    m_material.Shininess = shininess;
}
