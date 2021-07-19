#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H

#include <QOpenGLFunctions>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QDebug>

class WavefrontOBJ
{
public:
    WavefrontOBJ(){}

    bool load(const QString &path,
              QVector<QVector3D>& vertices, QVector<GLushort>& vertexIndices,
              QVector<QVector2D> &uvs, QVector<GLushort> &uvIndices,
              QVector<QVector3D> &normals, QVector<GLushort> &normalIndices)
    {
        if (!QFile::exists(path))
        {
            m_errorMsg = "File does not exist";
            return false;
        }

        auto lines = open(path);
        if (lines.empty())
        {
            m_errorMsg = "No 3D geometry data";
            return false;
        }


        QVector<QVector3D> tempVertices, tempNormals;
        QVector<QVector2D> tempUvs;


        foreach (QString line, lines) {

            if (line.left(2) == "v ")       // Vertex
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

                // 配列の0番目の要素はデータタイプなのでスキップ
                for (int i = 1; i < split.size(); i++) {
                    indices = split[i].split("/");
                    if ( !vertices.empty())
                    {
                        vertexIndices.append(static_cast<GLushort>(indices[0].toInt() - 1));
                    }

                    if ( !uvs.empty())
                    {
                        uvIndices.append(static_cast<GLushort>(indices[1].toInt() - 1));
                    }

                    if ( !normals.empty())
                    {
                        normalIndices.append(static_cast<GLushort>(indices[2].toInt() - 1));
                    }
                }
            }
        }

        return true;
    }

    QString errorMessage(){ return m_errorMsg; }

private:
    QString m_errorMsg;

    QList<QString> open(const QString &filename)
    {
        QList<QString> lines;

        QFile file(filename);
        if (! file.open(QIODevice::ReadOnly)) {
            m_errorMsg = file.errorString();
            return lines;
        }

        QTextStream in(&file);
        QString inStr = in.readAll();
        file.close();

        lines = inStr.split("\n");

        return lines;
    }

};

#endif // WAVEFRONTOBJ_H
