#ifndef OTHERTHREAD_H
#define OTHERTHREAD_H
#include <QThread>
#include <QMutex>
class myThread : public QThread
{
    Q_OBJECT
    mutable QMutex m_mutex;
    bool m_stop;
    int latency;
public:
    explicit myThread(QObject *parent = 0);

    void stop();

    void setLatency(int);

    void run() override;

    bool isStopped();

signals:
    void makeStep();
};

#endif // OTHERTHREAD_H
