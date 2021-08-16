#ifndef GLDEBUG_H
#define GLDEBUG_H

#include <QDebug>
#include <QLabel>
#include <QVector3D>

class GLDebug
{
public:
    explicit GLDebug(QWidget *parent = nullptr)
    {
        m_fps = new QLabel("FPS: ", parent);
        m_active = new QLabel("ActiveModel: ", parent);
        m_translation = new QLabel("Translation: ", parent);
        m_rotation = new QLabel("Rotation: ", parent);
        m_scale = new QLabel("Scale: ", parent);
        m_mouse = new QLabel("Mouse: ", parent);

        m_fps->setStyleSheet("QLabel { color : white; }");
        auto stylesheet = m_fps->styleSheet();
        m_active->setStyleSheet(stylesheet);
        m_translation->setStyleSheet(stylesheet);
        m_rotation->setStyleSheet(stylesheet);
        m_scale->setStyleSheet(stylesheet);
        m_mouse->setStyleSheet(stylesheet);

        int w = 400, h = m_fps->height();
        int cnt = 1;
        m_fps->setGeometry(10, h * cnt++, w, h);
        m_active->setGeometry(10, h * cnt++, w, h);
        m_translation->setGeometry(10, h * cnt++, w, h);
        m_rotation->setGeometry(10, h * cnt++, w, h);
        m_scale->setGeometry(10, h * cnt++, w, h);
        m_mouse->setGeometry(10, h * cnt++, w, h);
    }

    void update(const double fps, const int active, const QVector3D translation, const QVector3D angle, const float scale, const QVector3D mouse)
    {
        float dscale = (scale == 0.0f) ? 0.0f : scale * 100.0f;

        m_fps->setText(QString("FPS %1").arg(fps));
        m_active->setText(QString("ActiveModel: %1").arg(active));
        m_translation->setText(QString("Translation X:%1, Y:%2, Z:%3")
                                    .arg(static_cast<double>(translation.x()))
                                    .arg(static_cast<double>(translation.y()))
                                    .arg(static_cast<double>(translation.z())));
        m_rotation->setText(QString("Rotation X:%1, Y:%2, Z:%3")
                                 .arg(static_cast<double>(angle.x()))
                                 .arg(static_cast<double>(angle.y()))
                                 .arg(static_cast<double>(angle.z())));
        m_scale->setText(QString("Scale %1%").arg(static_cast<double>(dscale)));
        m_mouse->setText(QString("Camera X:%1, Y:%2, Z:%3")
                              .arg(static_cast<double>(mouse.x()))
                              .arg(static_cast<double>(mouse.y()))
                              .arg(static_cast<double>(mouse.z())));
    }

private:
    QLabel* m_fps;
    QLabel* m_active;
    QLabel* m_translation;
    QLabel* m_rotation;
    QLabel* m_scale;
    QLabel* m_mouse;
};

#endif // GLDEBUG_H
