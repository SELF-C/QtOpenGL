#ifndef BLENDER_H
#define BLENDER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QDebug>

class Blender
{
public:
    Blender();

    //bool loadObj(const QString &path, QVector<QVector3D> vertices, QVector<QVector2D> uvs, QVector<QVector3D> normals)
    bool loadObj(const QString &path)
    {
        if (!QFile::exists(path))
            return false;

        auto lines = openObj(path);

        if (lines.empty())
            return false;

        QVector<QVector3D> vertices, normals;
        QVector<QVector2D> uvs;
        foreach (QString line, lines) {

            if (line.left(2) == "v ")        // Vertex
            {
                QStringList split = line.split(" ");
                vertices.append(QVector3D(split[1].toFloat(), split[2].toFloat(), split[3].toFloat()));
            }
            else if (line.left(2) == "vt")  // UV
            {
                QStringList split = line.split(" ");
                uvs.append(QVector2D(split[1].toFloat(), split[2].toFloat()));
            }
            else if (line.left(2) == "vn")  // Normal
            {
                QStringList split = line.split(" ");
                normals.append(QVector3D(split[1].toFloat(), split[2].toFloat(), split[3].toFloat()));
            }
            else if (line.left(1) == "f")   // Face
            {
                QStringList split = line.split(" ");
                QStringList indices;

                /* triangul 1*/
                indices = split[1].split("/");
                m_vertexIndices->append(indices[0].toInt());
                m_uvIndices->append(indices[1].toInt());
                m_normalIndices->append(indices[2].toInt());

                /* triangul 2*/
                indices = split[2].split("/");
                m_vertexIndices->append(indices[0].toInt());
                m_uvIndices->append(indices[1].toInt());
                m_normalIndices->append(indices[2].toInt());

                /* triangul 2*/
                indices = split[3].split("/");
                m_vertexIndices->append(indices[0].toInt());
                m_uvIndices->append(indices[1].toInt());
                m_normalIndices->append(indices[2].toInt());
            }

        }

        qDebug() << vertices;
        qDebug() << uvs;
        qDebug() << normals;
        qDebug() << m_vertexIndices->toList();
        qDebug() << m_uvIndices;
        qDebug() << m_normalIndices;

        return true;
    }

private:
    QVector<int> m_vertexIndices[3];
    QVector<int> m_uvIndices[3];
    QVector<int> m_normalIndices[3];


    QList<QString> openObj(const QString &filename)
    {
        QList<QString> lines;

        QFile file(filename);
        if (! file.open(QIODevice::ReadOnly)) {
            qDebug() << file.errorString();
            return lines;
        }

        QTextStream in(&file);
        QString inStr = in.readAll();
        file.close();

        lines = inStr.split("\n");

        return lines;
    }

};

#endif // BLENDER_H
