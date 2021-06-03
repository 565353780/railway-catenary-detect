#include "RemoveFileThread.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include <QDebug>

RemoveFileThread::RemoveFileThread(QObject *parent):
    QThread(parent)
{

}

void RemoveFileThread::setRemoveFile(QString path)
{
    src_=path;
}

void RemoveFileThread::run()
{
    QTime time;time.start();
    bool success=false;
    QFileInfo info(src_);
    if(info.isFile()){
        success=QFile::remove(src_);
    }
    else if (info.isDir()) {
        QDir dir(src_);
        success=dir.removeRecursively();
    }
    qDebug()<<QString("remove %4 from %1 %2, success?:%3, using %5 ms").arg(src_).arg("").arg(success).arg(info.isFile()?"File":"Dir").arg(time.elapsed());
    emit signalFinishRemove(src_,success);
}
