#include "model.h"
#include "QDebug"

Model::Model()
{
    initialize();
}

Model::~Model()
{
    release();
}

void Model::initialize()
{
    initializeOpenGLFunctions();

    // メッシュデータの初期設定
    m_translation = QVector3D(0.0f, 0.0f, 0.0f);
    m_rotation = QQuaternion::fromEulerAngles(QVector3D(0.0f, 0.0f, 0.0f));
    m_scale = QVector3D(1.0f, 1.0f, 1.0f);

    // ライティングの初期設定
    m_light.Position = QVector4D(-25.0f, 25.0f, 25.0f, 1.0f);
    m_light.La = QVector3D(0.3f, 0.3f, 0.3f);
    m_light.Ld = QVector3D(0.8f, 0.8f, 0.8f);
    m_light.Ls = QVector3D(0.8f, 0.8f, 0.8f);

    m_material.Ka = QVector3D(0.8f, 0.8f, 0.8f);
    m_material.Kd = QVector3D(0.8f, 0.8f, 0.8f);
    m_material.Ks = QVector3D(0.8f, 0.8f, 0.8f);
    m_material.Shininess = 100.0f;

    // ステータス
    m_created = false;
    m_enabled = true;

    m_shaderProgram = new QOpenGLShaderProgram();
}

void Model::release()
{
    // And now release all OpenGL resources
    m_shaderProgram->release();
    m_shaderProgram->removeAllShaders();
    delete m_shaderProgram;
    m_shaderProgram=nullptr;

    // VBO release
    m_vbo.release();
    m_vbo.destroy();
}

bool Model::load(const QString &filename)
{
    QVector<Triangle3D> triangles;
    QStringList comments;

    // objファイルの読み込み
    if (!WavefrontOBJ().parser(filename, comments, triangles))
        return false;

    m_vertices.clear();
    m_indexes.clear();
    m_comments.clear();

    GLushort index = 0;
    for(int i = 0; i < triangles.count(); i++)
    {
        m_vertices.append(VertexData{ triangles.at(i).p1, triangles.at(i).p1Normal, triangles.at(i).p1TexCoord });
        m_vertices.append(VertexData{ triangles.at(i).p2, triangles.at(i).p2Normal, triangles.at(i).p2TexCoord });
        m_vertices.append(VertexData{ triangles.at(i).p3, triangles.at(i).p3Normal, triangles.at(i).p3TexCoord });
        m_indexes.append(index++);
        m_indexes.append(index++);
        m_indexes.append(index++);
    }
    m_comments = comments;

    return true;
}

void Model::bind(const QString &vertexShader, const QString &fragmentShader)
{
    shaderInit(vertexShader, fragmentShader);
    bufferInit();
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
    // 頂点バッファを生成
    m_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo.create();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.bind();
    m_vbo.allocate(m_vertices.constData(), m_vertices.size() * static_cast<int>(sizeof(VertexData)));
    m_vbo.release();

    // インデックスバッファを生成
    m_ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_ibo.create();
    m_ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ibo.bind();
    m_ibo.allocate(m_indexes.constData(), m_indexes.size() * static_cast<int>(sizeof(GLushort)));
    m_ibo.release();

    // インデックスバッファを生成したので頂点情報をクリア
    m_vertices.clear();

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

void Model::update()
{
    /* Model Matrix */



}

void Model::draw(const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &parentModelMatrix)
{
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_translation);
    modelMatrix.rotate(m_rotation);
    modelMatrix.scale(m_scale);
    modelMatrix = parentModelMatrix * modelMatrix;

    // Draw
    if ( m_enabled )
    {
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

        m_vbo.bind();
        m_ibo.bind();

        int offset = 0;
        m_shaderProgram->enableAttributeArray("VertexPosition");
        m_shaderProgram->setAttributeBuffer("VertexPosition", GL_FLOAT, offset, 3, sizeof(VertexData));

        offset += sizeof(QVector3D);
        m_shaderProgram->enableAttributeArray("VertexNormal");
        m_shaderProgram->setAttributeBuffer("VertexNormal", GL_FLOAT, offset, 3, sizeof(VertexData));

        glDrawElements(GL_TRIANGLES, m_indexes.size(), GL_UNSIGNED_SHORT, nullptr);

        m_ibo.release();
        m_vbo.release();
        m_shaderProgram->release();
    }

    for (int i = 0; i < m_children.size(); ++i) {
        m_children[i]->draw(projectionMatrix, viewMatrix, modelMatrix);
    }
}

void Model::setChild(int index, Model* child)
{
    if(m_children.size() < index && 0 > index)
    {
        qDebug() << "index out of range";
        return;
    }
    m_children[index] = child;
}

void Model::addChild(Model* child)
{
    m_children.append(child);
}

void Model::setRotation(QQuaternion rotation)
{
    m_rotation = rotation;
}

void Model::setTranslation(QVector3D translation)
{
    m_translation = translation;
}

void Model::setScale(float s)
{
    m_scale = QVector3D(s, s, s);
}

void Model::setScale(QVector3D s)
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

QOpenGLShaderProgram *Model::getShaderProgram() const
{
    return m_shaderProgram;
}

void Model::setShaderProgram(QOpenGLShaderProgram *shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

QVector<Model::VertexData> Model::getVertices() const
{
    return m_vertices;
}

void Model::setVertices(const QVector<VertexData> &vertices)
{
    m_vertices = vertices;
}

QOpenGLBuffer Model::getVbo() const
{
    return m_vbo;
}

void Model::setVbo(const QOpenGLBuffer &vbo)
{
    m_vbo = vbo;
}

QStringList Model::getComments() const
{
    return m_comments;
}

void Model::setComments(const QStringList &comments)
{
    m_comments = comments;
}

bool Model::getEnabled() const
{
    return m_enabled;
}

void Model::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

QOpenGLBuffer Model::getIbo() const
{
    return m_ibo;
}

void Model::setIbo(const QOpenGLBuffer &ibo)
{
    m_ibo = ibo;
}

Model::Light Model::getLight() const
{
    return m_light;
}

Model::Material Model::getMaterial() const
{
    return m_material;
}
