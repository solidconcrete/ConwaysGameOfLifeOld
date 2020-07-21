#include "myThread.h"
#include <QDebug>
#include <QtCore>
#include "dialog.h"
myThread::myThread(QObject *parent)
    : QThread(parent), m_stop(false), latency(1000){}

void myThread::run()
{
    m_stop = false;
    qDebug() << "thread started";
    while (!isStopped())
    {
//        const QMutexLocker locker(&m_mutex);
        emit makeStep();
        msleep(latency);
    }
    qDebug() << "thread stopped";
}
bool myThread::isStopped()
{
    const QMutexLocker locker(&m_mutex);
    return m_stop;
}

void myThread::stop()
{
    const QMutexLocker locker(&m_mutex);
    m_stop = true;
}

void myThread::setLatency(int newSpeed)
{
    const QMutexLocker locker(&m_mutex);
    latency = newSpeed;
}
