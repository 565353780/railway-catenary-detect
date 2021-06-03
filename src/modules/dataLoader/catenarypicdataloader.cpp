#include "catenarypicdataloader.h"

#include <QCryptographicHash>


#include "modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"

CatenaryPicDataLoader::CatenaryPicDataLoader(RailwayCatenaryDataManager* database_manager, QObject *parent)
    : QObject(parent),database_manager_(database_manager)
{

}

CatenaryPicDataLoader::~CatenaryPicDataLoader()
{

}

bool CatenaryPicDataLoader::loadPic(QString filePath)
{
    bool success=false;
    QMap<QString,QString> pic_attr;
    QString current_date_time=QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss");
    QFile pic_file(filePath);
    qDebug()<<"insert"<<filePath ;
    if(pic_file.exists()){
        pic_file.open(QFile::ReadOnly);
        if(pic_file.isOpen()){
            pic_attr.insert("Time",current_date_time);
            pic_attr.insert("imagepath",filePath);
            pic_attr.insert("PicID",QString::fromUtf8(QCryptographicHash::hash(pic_file.readAll(),QCryptographicHash::Md5).toBase64()).left(32));
            pic_attr.insert("isPredicted","0");

            success=database_manager_->insertRecord2Pic(pic_attr);
            qDebug()<<success;
        }
    }
    else{
        qDebug()<<filePath<<"not exist";
    }
    return success;
}

bool CatenaryPicDataLoader::loadPicsFromDir(QString dirPath)
{
    bool success=false;
    QDir dir(dirPath);
    if(dir.exists()){
        QStringList pic_list=dir.entryList(QStringList()<<"*.jpg"<<"*.png",QDir::Files);
        foreach (QString filename, pic_list) {
            success=loadPic(dir.filePath(filename));
        }
    }
    return success;
}
