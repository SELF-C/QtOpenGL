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
#include <QRegularExpression>

struct Triangle3D
{
    QVector3D p1, p2, p3;
    QVector3D p2Normal, p1Normal, p3Normal;
    QVector2D p1UV, p2UV, p3UV;
    QVector<GLushort> vertexIndices, uvIndices, normalIndices;
};

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

    void parser(const QString &fileName, QStringList &comments, QVector<Triangle3D> &triangles)
    {
        comments.clear();
        triangles.clear();

        QFile file(fileName);
        if(!file.exists())
            return;

        QVector<QVector3D> v, vn;
        QVector<QVector2D> vt;

        if (!file.open(QFile::ReadOnly | QFile::Text))
            return;

        while(!file.atEnd())
        {
            QString line = file.readLine().trimmed();
            QStringList lineParts = line.split(QRegularExpression("\\s+"));
            if(lineParts.count() > 0)
            {
                // コメントなら
                if(lineParts.at(0).compare("#", Qt::CaseInsensitive) == 0)
                {
                    comments.append(line.remove(0, 1).trimmed());
                }
                // 頂点位置の場合(v)
                else if(lineParts.at(0).compare("v", Qt::CaseInsensitive) == 0)
                {
                    v.append(QVector3D(lineParts.at(1).toFloat(),
                                       lineParts.at(2).toFloat(),
                                       lineParts.at(3).toFloat()));
                }
                // UV座標の場合(vt)
                else if(lineParts.at(0).compare("vt", Qt::CaseInsensitive) == 0)
                {
                    vt.append(QVector2D(lineParts.at(1).toFloat(),
                                        lineParts.at(2).toFloat()));
                }
                // 法線位置の場合(vn)
                else if(lineParts.at(0).compare("vn", Qt::CaseInsensitive) == 0)
                {
                    vn.append(QVector3D(lineParts.at(1).toFloat(),
                                        lineParts.at(2).toFloat(),
                                        lineParts.at(3).toFloat()));
                }
                // 面データの場合。すべて三角形であるとする。
                else if(lineParts.at(0).compare("f", Qt::CaseInsensitive) == 0)
                {
                    Triangle3D triangle;

                    // 頂点座標を取得
                    triangle.p1 = v.at(lineParts.at(1).split("/").at(0).toInt() - 1);
                    triangle.p2 = v.at(lineParts.at(2).split("/").at(0).toInt() - 1);
                    triangle.p3 = v.at(lineParts.at(3).split("/").at(0).toInt() - 1);

                    if(vt.count() > 0) // UV座標があるか確認
                    {
                        triangle.p1UV = vt.at(lineParts.at(1).split("/").at(1).toInt() - 1);
                        triangle.p2UV = vt.at(lineParts.at(2).split("/").at(1).toInt() - 1);
                        triangle.p3UV = vt.at(lineParts.at(3).split("/").at(1).toInt() - 1);
                    }

                    // 法線位置を取得
                    triangle.p1Normal = vn.at(lineParts.at(1).split("/").at(2).toInt() - 1);
                    triangle.p2Normal = vn.at(lineParts.at(2).split("/").at(2).toInt() - 1);
                    triangle.p3Normal = vn.at(lineParts.at(3).split("/").at(2).toInt() - 1);

                    triangles.append(triangle);
                }

            }
        }
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
