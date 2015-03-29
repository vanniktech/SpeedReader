#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
    using QThread::run;
public:
    explicit Thread(QObject *parent = 0);
    ~Thread();

signals:

public slots:
};

#endif // WORKERTHREAD_H
