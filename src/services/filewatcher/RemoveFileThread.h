#ifndef REMOVEFILETHREAD_H
#define REMOVEFILETHREAD_H

#include <QThread>

class RemoveFileThread : public QThread
{
    Q_OBJECT
public:
    RemoveFileThread(QObject* parent=nullptr);
    void setRemoveFile(QString path);
private:
    QString src_;
public:
    virtual void run();
signals:
    void signalFinishRemove(QString , bool);
};

#endif // REMOVEFILETHREAD_H
