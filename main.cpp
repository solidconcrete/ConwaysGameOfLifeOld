#include "dialog.h"

#include <QApplication>

class Work : public QRunnable
{
public:
    void run()
    {
        qDebug() << "Hello from thread "<< QThread::currentThread();
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Dialog w;
//    w.show();
    Work work;
    work.setAutoDelete(false);
    QThreadPool *threadPool = QThreadPool::globalInstance();
    threadPool->start(&work);
    qDebug() << "Hello from GUI thread " << QThread::currentThread();
    threadPool->waitForDone();
    return a.exec();
}
