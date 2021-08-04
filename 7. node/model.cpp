#include "model.h"
#include "QDebug"

Model::Model()
{
    initializeOpenGLFunctions();
    initialize();
}

Model::~Model()
{
    // And now release all OpenGL resources
    m_shaderProgram->release();
    m_shaderProgram->removeAllShaders();
    delete m_shaderProgram;
    m_shaderProgram=nullptr;

    // VBO release
    m_vbo.vertex.release();
    m_vbo.vertex.destroy();

    m_vbo.normal.release();
    m_vbo.normal.destroy();

    m_vbo.uv.release();
    m_vbo.uv.destroy();
}

void Model::initialize()
{
    // メッシュデータの初期設定
    m_transform.rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 0.0f), 0.0f);
    m_transform.translation = QVector3D(0.0f, 0.0f, 0.0f);
    m_transform.scale = 1.0f;

    // ライティングの初期設定
    m_light.Position = QVector4D(-25.0f, 25.0f, 25.0f, 1.0f);
    m_light.La = QVector3D(0.3f, 0.3f, 0.3f);
    m_light.Ld = QVector3D(0.8f, 0.8f, 0.8f);
    m_light.Ls = QVector3D(0.8f, 0.8f, 0.8f);

    m_material.Ka = QVector3D(0.8f, 0.8f, 0.8f);
    m_material.Kd = QVector3D(0.8f, 0.8f, 0.8f);
    m_material.Ks = QVector3D(0.8f, 0.8f, 0.8f);
    m_material.Shininess = 100.0f;

    m_shaderProgram = new QOpenGLShaderProgram();

    m_created = false;
}

bool Model::load(const QString &filename)
{
    QVector<Triangle3D> triangles;
    QStringList comments;

    // objファイルの読み込み
    if (!WavefrontOBJ().parser(filename, comments, triangles))
        return false;

    m_vertices.clear();
    m_normals.clear();
    m_uvs.clear();
    m_comments.clear();

    for(int i = 0; i < triangles.count(); i++)
    {
        m_vertices.append(triangles.at(i).p1);
        m_vertices.append(triangles.at(i).p2);
        m_vertices.append(triangles.at(i).p3);

        m_normals.append(triangles.at(i).p1Normal);
        m_normals.append(triangles.at(i).p2Normal);
        m_normals.append(triangles.at(i).p3Normal);
    }
    m_comments = comments;

    qDebug() << m_vertices.toList();
    qDebug() << "m_normals";
    qDebug() << m_normals.size();
    qDebug() << m_normals.toList();

    return true;
}

void Model::bind(const QString &vertexShader, const QString &fragmentShader)
{
    shaderInit(vertexShader, fragmentShader);
    bufferInit();
    m_created = true;
}

void Model::shaderInit(const QString &vertexShaderFile, const QString &fragmentShaderFile)
{
    // シェーダーのコンパイル
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile);
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile);

    // シェーダプログラムをリンク
    m_shaderProgram->link();
}

void Model::bufferInit()
{
    // 頂点情報をVBOに転送する
    m_vbo.vertex = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo.vertex.create();
    m_vbo.vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.vertex.bind();
    m_vbo.vertex.allocate(m_vertices.constData(), m_vertices.size() * static_cast<int>(sizeof(QVector3D)));
    m_vbo.vertex.release();

    // 法線情報をVBOに転送する
    m_vbo.normal = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo.normal.create();
    m_vbo.normal.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.normal.bind();
    m_vbo.normal.allocate(m_normals.constData(), m_normals.size() * static_cast<int>(sizeof(QVector3D)));
    m_vbo.normal.release();

    //　テクスチャマッピングをVBOに転送する
    m_vbo.uv = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo.uv.create();
    m_vbo.uv.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.uv.bind();
    m_vbo.uv.allocate(m_uvs.constData(), m_uvs.size() * static_cast<int>(sizeof(QVector3D)));
    m_vbo.uv.release();

    // シェーダーで使用する属性の設定
    m_shaderProgram->bind();
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
}

void Model::draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix)
{
    /* Model Matrix */
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_transform.translation);
    modelMatrix.rotate(m_transform.rotation);
    modelMatrix.scale(m_transform.scale);

    // Draw
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

    m_vbo.vertex.bind();
    m_shaderProgram->enableAttributeArray("VertexPosition");
    m_shaderProgram->setAttributeBuffer("VertexPosition", GL_FLOAT, 0, 3);

    m_vbo.normal.bind();
    m_shaderProgram->enableAttributeArray("VertexNormal");
    m_shaderProgram->setAttributeBuffer("VertexNormal", GL_FLOAT, 0, 3);

    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

    m_vbo.normal.release();
    m_vbo.vertex.release();
    m_shaderProgram->release();
}

void Model::setRotation(float angleX, float angleY, float angleZ)
{
    m_transform.rotation =
            QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angleZ) *
            QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), angleX) *
            QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angleY);
}

void Model::setTranslation(float x, float y, float z)
{
    m_transform.translation = QVector3D(x, y, z);
}

void Model::setScale(float s)
{
    m_transform.scale = s;
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

QOpenGLShaderProgram *Model::getShaderProgram() const
{
    return m_shaderProgram;
}

void Model::setShaderProgram(QOpenGLShaderProgram *shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

QVector<QVector3D> Model::getVertices() const
{
    return m_vertices;
}

void Model::setVertices(const QVector<QVector3D> &vertices)
{
    m_vertices = vertices;
}

QVector<QVector3D> Model::getNormals() const
{
    return m_normals;
}

void Model::setNormals(const QVector<QVector3D> &normals)
{
    m_normals = normals;
}

QVector<QVector2D> Model::getUvs() const
{
    return m_uvs;
}

void Model::setUvs(const QVector<QVector2D> &uvs)
{
    m_uvs = uvs;
}

QStringList Model::getComments() const
{
    return m_comments;
}

void Model::setComments(const QStringList &comments)
{
    m_comments = comments;
}

QOpenGLBuffer Model::getVertex() const
{
    return m_vbo.vertex;
}

void Model::setVertex(const QOpenGLBuffer &vertex)
{
    m_vbo.vertex = vertex;
}

QOpenGLBuffer Model::getNormal() const
{
    return m_vbo.normal;
}

void Model::setNormal(const QOpenGLBuffer &normal)
{
    m_vbo.normal = normal;
}

QOpenGLBuffer Model::getUv() const
{
    return m_vbo.uv;
}

void Model::setUv(const QOpenGLBuffer &uv)
{
    m_vbo.uv = uv;
}

Model::Light Model::getLight() const
{
    return m_light;
}

Model::Material Model::getMaterial() const
{
    return m_material;
}
