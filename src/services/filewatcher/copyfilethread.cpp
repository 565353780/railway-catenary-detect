#include "copyfilethread.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTime>



CopyFileThread::CopyFileThread(QObject *parent):
    QThread(parent),src_(""),dst_("")
{

}

void CopyFileThread::setSrcDst(QString src, QString dst)
{
    src_=src;
    dst_=dst;
}

void CopyFileThread::run()
{
    if(src_.isEmpty()|| dst_.isEmpty()) return;
    QFileInfo srcInfo(src_);
    bool success=false;
    QTime time;time.start();
    if(srcInfo.exists()){
        if(srcInfo.isDir()){
            QDir dir;
            bool mkdir_success=dir.mkpath(dst_);
            if(mkdir_success){
                success=copyDir(src_,dst_);
            }
        }
        else if(srcInfo.isFile()) {
            success=QFile::copy(src_,dst_);
        }
    }
    qDebug()<<QString("copy %4 from %1 to %2, success?:%3, using %5 ms").arg(srcInfo.fileName().leftJustified(10,'.',true)).arg(dst_.rightJustified(20,'.',true)).arg(success).arg(srcInfo.isFile()?"File":"Dir").arg(time.elapsed());
    emit signalFinishCopy(src_,dst_,success);
    src_="";
    dst_="";
}
