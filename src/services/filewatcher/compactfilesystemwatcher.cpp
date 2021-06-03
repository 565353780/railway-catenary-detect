#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QTime>
#include <QString>
#include <QTimer>
#include "compactfilesystemwatcher.h"



FileSystemWatcher::FileSystemWatcher(QObject *parent)
    : QObject(parent)
{
    last_modified_time_=QDateTime::currentDateTime();
    if (true)
    {
        my_system_watcher_ = new QFileSystemWatcher(this);

        // 连接QFileSystemWatcher的directoryChanged和fileChanged信号到相应的槽
        connect(my_system_watcher_, SIGNAL(directoryChanged(QString)), this, SLOT(directoryUpdated(QString)));
        connect(my_system_watcher_, SIGNAL(fileChanged(QString)), this, SLOT(fileUpdated(QString)));
    }
}

FileSystemWatcher::~FileSystemWatcher()
{
    clearPaths();
    delete my_system_watcher_;
}

void FileSystemWatcher::clearPaths()
{
    my_system_watcher_->removePaths(my_system_watcher_->directories());
    my_system_watcher_->removePaths(my_system_watcher_->files());
}

void FileSystemWatcher::removePath(QString &path)
{
    my_system_watcher_->removePath(path);
}


// 监控文件或目录
void FileSystemWatcher::addWatchPath(QString path, bool watchfile)
{

    // 添加监控路径
    bool add_path_state=my_system_watcher_->addPath(path);
    qDebug()<<"add path "<<path<<" state: "<<add_path_state;

    // 如果添加路径是一个目录，保存当前内容列表
    QFileInfo file(path);
    if (file.isDir())
    {
        const QDir dirw(path);
        m_currentContentsMap[path] = dirw.entryList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        watchfile;//for adding file recursively
    /*
        if(watchfile){
//                            bool add_path_state=m_pSystemWatcher->addPath(path+"0");
//                            qDebug()<<"add path "<<path+"0"<<" state: "<<add_path_state;
            QFile out(path+"0.txt");
            out.open(QIODevice::WriteOnly);
            QTextStream outStream(&out);
            outStream<<0;
            //m_pSystemWatcher->addPath(path+"0.txt");
//            foreach (QFileInfo filePath, dirw.entryInfoList(QDir::Files|QDir::NoDotDot)) {
//                add_path_state=m_pSystemWatcher->addPath(filePath.absoluteFilePath());
//                qDebug()<<"add path "<<filePath.absoluteFilePath()<<" state: "<<add_path_state;
//                break;
//            }
            out.close();
            QFile::remove(path+"0.txt");

        }
    */
    }
}

void copyPath(QString const &src , QString dst)
{
    QDir dir(src);
    if (! dir.exists())
        return;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        copyPath(src+ QDir::separator() + d, dst_path);
    }
    foreach (QString f, dir.entryList(QDir::Files)) {
        //qDebug()<<"file i"<<f;
        QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
    }

}



void FileSystemWatcher::searchForAllFileHeading(QString heading, QString DirPath)
{
    QDir dir(DirPath);
    qDebug()<<"searching in path"<<DirPath;
    QFileInfoList fileList=dir.entryInfoList(QDir::Files);

    QFileInfoList dirList=dir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);
    foreach (QFileInfo info, fileList) {
        if( info.baseName().startsWith(heading)){
            qDebug()<<"find file"<<info.absoluteFilePath();
        }
    }
    foreach (QFileInfo info, dirList) {
        searchForAllFileHeading(heading, info.absoluteFilePath());
    }
}




// 只要任何监控的目录更新（添加、删除、重命名），就会调用。
void FileSystemWatcher::directoryUpdated(const QString &path)
{
    QTime time;time.start();
    QString dirUpdated= QString("Directory updated: %1").arg(path);
//    qDebug()<<dirUpdated;
    //emit fileMoifyInfo(dirUpdated);
    // 比较最新的内容和保存的内容找出区别(变化)
    QStringList last_entry_list = m_currentContentsMap[path];
    QDir dir(path);
    dir.setNameFilters(file_suffix_filters_);
    QStringList newEntryList = dir.entryList( QDir::Files| QDir::AllDirs |QDir::NoDotAndDotDot);
    int current_file_num=newEntryList.size();
    qDebug()<<"entry num:"<<current_file_num;
    emit signalDirFileNum(path,current_file_num);
    if(true){
        QSet<QString> newDirSet = QSet<QString>::fromList(newEntryList);
        QSet<QString> currentDirSet = QSet<QString>::fromList(last_entry_list);

        // 添加了文件
        QSet<QString> newFiles = newDirSet - currentDirSet;
        QStringList newFile = newFiles.toList();

        // 文件已被移除
        QSet<QString> deletedFiles = currentDirSet - newDirSet;
        QStringList deleteFile = deletedFiles.toList();

        // 更新当前设置
        m_currentContentsMap[path] = newEntryList;

        if (!newFile.isEmpty() && !deleteFile.isEmpty())
        {
            //qDebug()<<"both add and delete, \nadded--"<<newFile.join(",")<<";\ndeleted--"<<deleteFile.join(",");
        }
        if(true)
        {
            // 添加新文件/目录至Dir
            if (!newFile.isEmpty())
            {
                qDebug() << "New Files/Dirs added: " << newFile;

                foreach (QString file, newFile)
                {
                    // 处理操作每个新文件....
                    QFileInfo info(path+file);
                    emit signalFileOperationInfo(path,QString("add"),info.suffix(),file);
                    if(info.isDir()){

                    }
                }

            }

//            // 从Dir中删除文件/目录
//            if (!deleteFile.isEmpty())
//            {
//                qDebug() << "Files/Dirs deleted: " << deleteFile;

//                foreach(QString file, deleteFile)
//                {
//                    //处理操作每个被删除的文件....
//                    QFileInfo info(path+file);
//                    emit signalFileOperationInfo(path,QString("delete"),info.suffix(),file);
//                }
//            }


            /*
            //file content change
            if(newFile.isEmpty()&& deleteFile.isEmpty()){
                QFileInfoList infoList=dir.entryInfoList(QDir::Files,QDir::Time);
                QFileInfo info_find;

                foreach (QFileInfo info, infoList) {
                    if(info.metadataChangeTime()>lastModifiedTime ||info.lastModified()>lastModifiedTime){
                        info_find=info;
                        break;
                    }
                }
                if (info_find.exists())
                    emit signalFileOperationInfo(path,QString("modify"),info_find.suffix(),info_find.fileName());
                if(info_find.isDir()){
    //                copyPath(path+QDir::separator()+info.fileName(),save_path_+QDir::separator()+info.baseName()+"@"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
                }
                else if(info_find.isFile()) {
                    bool find=true;
                    if(find){
                        lastModifiedTime=QDateTime::currentDateTime();
                    }
                }

            }
            */
        }
    }
    int handle_time=time.elapsed();
    if(handle_time>100){
        qDebug()<<"handle dir change time: "<<handle_time<<"ms";
    }
}


// 文件修改时调用
void FileSystemWatcher::fileUpdated(const QString &path)
{
    QFileInfo file(path);
    QString fileModify= QString("File Modified: %1").arg(path);
    qDebug()<< fileModify;
    if(file_suffix_filters_.contains(file.suffix()) && file.metadataChangeTime()>last_modified_time_){
        last_modified_time_=file.metadataChangeTime();
        emit signalFileOperationInfo(path,QString("modify"),file.suffix(),file.fileName());
    }

}

void FileSystemWatcher::set_dir_suffix_filters(const QStringList &dir_suffixs)
{
    dir_suffix_filters_ = dir_suffixs;
}

void FileSystemWatcher::set_file_suffix_filters(const QStringList &suffix_filters)
{
    file_suffix_filters_ = suffix_filters;
}



