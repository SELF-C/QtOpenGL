#ifndef FPSMANAGER_H
#define FPSMANAGER_H

#include <QElapsedTimer>
#include <QDebug>
class FpsManager
{
public:
    FpsManager(){
        m_fpsCountTimer.start();
        m_framesInSecond = 0;
        m_fps = 0;
    }


    void frameRateCalculator()
    {
        if (m_fpsCountTimer.elapsed() >= 1000)
        {
            m_fps = m_framesInSecond;
            m_framesInSecond = 0;
            m_fpsCountTimer.start();
        }
        m_framesInSecond++;
    }

    int getFps(){ return m_fps; }

private:
    QElapsedTimer m_fpsCountTimer;
    int m_framesInSecond;
    int m_fps;
};

#endif // FPSMANAGER_H
