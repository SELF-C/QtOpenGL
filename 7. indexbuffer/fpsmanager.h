#ifndef FPSMANAGER_H
#define FPSMANAGER_H

#include <QElapsedTimer>
#include <QtCore/QThread>
#include <QDebug>
#include <QCoreApplication>

class FpsManager
{
public:
    FpsManager(){
        m_framesInSecond = 0;
        m_fps = 0;
    }

    void frameRateCalculator()
    {
        // カウンターが開始されていなければスタートさせる
        if (!m_fpsCountTimer.isValid())
            m_fpsCountTimer.start();

        if (m_fpsCountTimer.elapsed() >= 1000)
        {
            m_fps = m_framesInSecond;
            m_framesInSecond = 0;
            m_fpsCountTimer.restart();
        }
        m_framesInSecond++;
    }

    void fixedFrameRate(int fixedFps = 60)
    {
        // ウェイト時間
        auto waitTime = m_framesInSecond * 1000 / fixedFps - m_fpsCountTimer.elapsed();
        if (waitTime > 0)
            QThread::msleep(static_cast<unsigned long>(waitTime));
    }

    int getFps(){ return m_fps; }
    QElapsedTimer getFpsCountTimer() const{ return m_fpsCountTimer; }

private:
    QElapsedTimer m_fpsCountTimer;
    int m_framesInSecond;
    int m_fps;
};

#endif // FPSMANAGER_H
