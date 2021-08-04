#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H

#include <QString>
#include <QFile>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QRegularExpression>
#include <QDebug>

struct Triangle3D
{
    QVector3D p1, p2, p3;
    QVector3D p2Normal, p1Normal, p3Normal;
    QVector2D p1TexCoord, p2TexCoord, p3TexCoord;
};

class WavefrontOBJ
{
public:
    WavefrontOBJ(){}

    bool parser(const QString &fileName, QStringList &comments, QVector<Triangle3D> &triangles)
    {
        comments.clear();
        triangles.clear();

        QFile file(fileName);
        if(!file.exists())
        {
            qWarning() << "File does not exist";
            return false;
        }

        QVector<QVector3D> v, vn;
        QVector<QVector2D> vt;

        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            qWarning() << "Can't open file";
            return false;
        }

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
                        triangle.p1TexCoord = vt.at(lineParts.at(1).split("/").at(1).toInt() - 1);
                        triangle.p2TexCoord = vt.at(lineParts.at(2).split("/").at(1).toInt() - 1);
                        triangle.p3TexCoord = vt.at(lineParts.at(3).split("/").at(1).toInt() - 1);
                    }

                    // 法線位置を取得
                    triangle.p1Normal = vn.at(lineParts.at(1).split("/").at(2).toInt() - 1);
                    triangle.p2Normal = vn.at(lineParts.at(2).split("/").at(2).toInt() - 1);
                    triangle.p3Normal = vn.at(lineParts.at(3).split("/").at(2).toInt() - 1);

                    triangles.append(triangle);
                }

            }
        }
        file.close();

        return true;
    }

    QString errorMessage(){
        return m_errorMsg;
    }

private:
    QString m_errorMsg="";
};

#endif // WAVEFRONTOBJ_H
