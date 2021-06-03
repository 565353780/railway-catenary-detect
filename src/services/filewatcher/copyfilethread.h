#ifndef COPYFILETHREAD_H
#define COPYFILETHREAD_H

#include <QThread>
#include <QDir>
#include <QFile>
class CopyFileThread :public QThread
{
    Q_OBJECT
public:
    CopyFileThread(QObject* parent=nullptr);
private:
    QString src_;
    QString dst_;
public:
    void setSrcDst(QString src, QString dst);
    virtual void run();
signals:
    void signalFinishCopy(QString src, QString dst, bool success);
public:
    static bool copyDir(QString src, QString dst)
    {
        QDir dir(src);
        if (! dir.exists())
            return false;

        foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString dst_path = dst + QDir::separator() + d;
            dir.mkpath(dst_path);
            copyDir(src+ QDir::separator() + d, dst_path);
        }
        foreach (QString f, dir.entryList(QDir::Files)) {
    //        qDebug()<<"file i"<<f;
            QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
        }
        return true;
    }
};

#endif // COPYFILETHREAD_H
