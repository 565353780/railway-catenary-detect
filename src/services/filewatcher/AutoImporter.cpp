#include "AutoImporter.h"

#include <QTime>
#include <QTimer>
#include <QString>

#include "copyfilethread.h"
#include "RemoveFileThread.h"
#include "compactfilesystemwatcher.h"

AutoImporter::AutoImporter(QObject *parent) : QObject(parent)
{

    copy_thread_=new CopyFileThread(this);
    connect(copy_thread_,&CopyFileThread::finished,this,&AutoImporter::slotActualBeginCopy);
    connect(copy_thread_,&CopyFileThread::signalFinishCopy,this, &AutoImporter::slotHandleCopyFinished);
    delay_timer_=new QTimer(this);
    delay_timer_->setInterval(30);
    delay_timer_->setSingleShot(true);
    connect(delay_timer_,&QTimer::timeout,this,&AutoImporter::slotActualBeginCopy);

    remove_thread_=new RemoveFileThread(this);
    connect(remove_thread_,&RemoveFileThread::signalFinishRemove,this,&AutoImporter::slotHandleRemoveFinished);
    remove_timer_=new QTimer(this);
    remove_timer_->setInterval(50);
    connect(remove_timer_,&QTimer::timeout,this,&AutoImporter::slotActualBeginRemove);


    copy_pcbimage_thread_=new CopyFileThread(this);
    connect(copy_pcbimage_thread_,&CopyFileThread::finished,this,&AutoImporter::slotActualBeginCopyPCBImage);
    connect(copy_pcbimage_thread_,&CopyFileThread::signalFinishCopy,this, &AutoImporter::slotHandleCopyPCBImageFinished);


    rename_timer_=new QTimer(this);
    rename_timer_->setInterval(50);
    connect(rename_timer_,&QTimer::timeout, this, &AutoImporter::slotRenameAndMakeDir);

    file_watcher_= new FileSystemWatcher(this);
    connect(file_watcher_,&FileSystemWatcher::signalFileOperationInfo,this,&AutoImporter::slotHandleFileOperationFound);
    connect(file_watcher_,&FileSystemWatcher::signalDirFileNum,this,&AutoImporter::slotHandleDirNum);
}

AutoImporter::~AutoImporter()
{
    copy_thread_->exit();
    file_watcher_->clearPaths();
}

QMap<QString, QString> AutoImporter::watch_path_dir_prefix() const
{
    return watch_path_key_value_;
}

void AutoImporter::set_watch_path_key_value(const QMap<QString, QString> &watch_path_key_value)
{
    watch_path_key_value_ = watch_path_key_value;
    file_watcher_->clearPaths();
    foreach (QString key, watch_path_key_value.keys()) {
        file_watcher_->addWatchPath(key);
    }
}

void AutoImporter::insert_watch_path_dir_prefix(QString watch_dir, QString prefix)
{
    watch_path_key_value_.insert(watch_dir,prefix);
    file_watcher_->addWatchPath(watch_dir,false);
    QDir dir;
    dir.mkpath(default_backup_path_+prefix);
}

void AutoImporter::remove_watch_path(QString path)
{
    watch_path_key_value_.remove(path);
    file_watcher_->removePath(path);
}

QString AutoImporter::default_backup_path() const
{
    return default_backup_path_;
}

void AutoImporter::set_default_backup_path(const QString &default_backup_path)
{
    default_backup_path_ = QDir::currentPath()+default_backup_path;
    QDir dir;
    dir.mkpath(default_backup_path_);
}

void AutoImporter::set_rename_entry_max_(int num)
{
    rename_dir_entry_max_=num;
    qDebug()<<"max entry num:"<<num;
}

void AutoImporter::testRename()
{
    QDir dir("E:/thunder_download");
    QTime time; time.start();
    dir.rename(dir.path(),dir.path()+"_new");
    qDebug()<<"rename time"<<time.elapsed()<<" ms";

}

void AutoImporter::pushPCBImageIntoCopy()
{
    if(watch_path_key_value_.values().contains("PCBImage"))
    {
        QString dir=watch_path_key_value_.key("PCBImage");
        QDir pcb_dir(dir);
        QStringList file_list=pcb_dir.entryList(QDir::Files);
        QDir backup_pcb_dir(QString("%1/%2")
                            .arg(default_backup_path_).arg("PCBImage"));
        QStringList backup_file_list=backup_pcb_dir.entryList(QDir::Files);
        QSet<QString> src_set=QSet<QString>::fromList(file_list);
        QSet<QString> dst_set=QSet<QString>::fromList(backup_file_list);
        QSet<QString> to_copy_set=src_set-dst_set;
        foreach (QString file, to_copy_set) {
            to_copy_pcbimage_src_dst_queue_.push_back(QPair<QString,QString>(dir+"/"+file,QString("%1/%2/%3")
                                                      .arg(default_backup_path_).arg("PCBImage").arg(file)));
            copy_pcbimage_thread_->start();
        }
    }
}

void AutoImporter::slotActualBeginCopy()
{
    if(!copy_thread_->isRunning() && to_copy_src_dst_queue_.size()>0){
        QPair<QString,QString> src_dst=to_copy_src_dst_queue_.front();
        QString src=src_dst.first, dst=src_dst.second;
        copy_thread_->setSrcDst(src,dst);
        copy_thread_->start();
    }

}

void AutoImporter::slotActualBeginRemove()
{
    if(!remove_thread_->isRunning() && to_remove_dst_queue_.size()>0){
        QString dst=to_remove_dst_queue_.front();
        remove_thread_->setRemoveFile(dst);
        remove_thread_->start();
    }
}

void AutoImporter::slotActualBeginCopyPCBImage()
{
    if(!copy_pcbimage_thread_->isRunning() &&to_copy_pcbimage_src_dst_queue_.size()>0){
        QPair<QString,QString> src_dst=to_copy_pcbimage_src_dst_queue_.front();
        QString src=src_dst.first, dst=src_dst.second;
        copy_pcbimage_thread_->setSrcDst(src,dst);
        copy_pcbimage_thread_->start();
    }
}

void AutoImporter::slotInsertToCopy(QString prefix, QString filename)
{
    QString dir=watch_path_key_value_.key(prefix);
    QString src,dst;
    src=dir+"/"+filename;
    dst=QString("%1/%2/%3")
            .arg(default_backup_path_).arg(prefix).arg(filename);
    to_copy_src_dst_queue_.push_back(QPair<QString,QString>(src,dst));

    slotActualBeginCopy();
}

void AutoImporter::slotInsertPCBToCopy(QString filename)
{
    QString prefix="PCBImage";
    QString dir=watch_path_key_value_.key(prefix);
    if(!dir.isEmpty()){
        QString src,dst;
        src=dir+"/"+filename;
        dst=QString("%1/%2/%3")
                .arg(default_backup_path_).arg(prefix).arg(filename);
        to_copy_pcbimage_src_dst_queue_.push_back(QPair<QString,QString>(src,dst));
        slotActualBeginCopyPCBImage();
    }
}

void AutoImporter::slotHandleCopyPCBImageFinished(QString src, QString dst, bool success)
{
    emit signalFinishCopy(dst,success,"PCBImage");
    if(to_copy_pcbimage_src_dst_queue_.size()>0)
        to_copy_pcbimage_src_dst_queue_.pop_front();
    slotActualBeginCopyPCBImage();
}

void AutoImporter::slotHandleCopyFinished(QString src, QString dst, bool success)
{
    if(to_copy_src_dst_queue_.size()<1) { return;}
    to_copy_src_dst_queue_.pop_front();
    QString prefix;
    foreach (QString key, watch_path_key_value_.keys()) {
        if(src.startsWith(key)){
            prefix=watch_path_key_value_.value(key);
            break;
        }
    }
    emit signalFinishCopy(dst,success,prefix);
}

void AutoImporter::slotHandleRemoveFinished(QString src, bool success)
{
    qDebug()<<"finish remove"<<src<<"success?:"<<success;
    to_remove_dst_queue_.pop_front();
}

void AutoImporter::slotHandleFileOperationFound(QString dir, QString operation, QString suffix, QString filename)
{
    QString prefix=watch_path_key_value_.value(dir);
    if(operation.compare("add")==0 ){
        QString src,dst;
        src=dir+filename;
        dst=QString("%1/%2/%3")
                .arg(default_backup_path_).arg(prefix).arg(filename);

        to_copy_src_dst_queue_.push_back(QPair<QString,QString>(src,dst));
        delay_timer_->start();
    }

}

void AutoImporter::slotHandleDirNum(QString dir, int num)
{
    if(num>rename_dir_entry_max_ && watch_path_key_value_.value(dir)=="Judge"){
//        QDir current_dir(dir);
//        QStringList fileList=current_dir.entryList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot,QDir::Time);
//        foreach(QString filename, fileList){
//            to_remove_dst_queue_<<dir+"/"+filename;
//        }
//        qDebug()<<"files to remove:"<<to_remove_dst_queue_.size();
//        remove_timer_->start();
        rename_dir_=dir;
        rename_timer_->start();
    }
}

void AutoImporter::slotRenameAndMakeDir()
{
    rename_success_=false;
    QString current_date_string=QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    //qDebug()<<current_date_string;
    QTime time; time.start();
    QDir origin_dir(rename_dir_);
    qDebug()<<"origin exist"<< origin_dir.exists();
    bool success=origin_dir.rename(origin_dir.path(),origin_dir.path()+"@"+current_date_string);
    QDir tmp_dir;
    tmp_dir.mkpath(rename_dir_+"/");
    file_watcher_->removePath(rename_dir_);
    file_watcher_->addWatchPath(rename_dir_);
    qDebug()<<"rename dir:"<< rename_dir_ << "time:"<<time.elapsed()<<" ms"<<" success?:"<<success;
    rename_success_=success;
    if(rename_success_ || rename_dir_.isEmpty()){
        rename_dir_="";
        rename_timer_->stop();
    }
}
