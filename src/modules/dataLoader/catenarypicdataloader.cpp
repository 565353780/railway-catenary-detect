#include "catenarypicdataloader.h"

#include <QCryptographicHash>
#include <QQmlContext>

#include "modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"

CatenaryPicDataLoader::CatenaryPicDataLoader(RailwayCatenaryDataManager* database_manager, QObject *parent)
    : QObject(parent),database_manager_(database_manager)
{
    review_json_dataloader_ = new ReviewJsonDataLoader(database_manager_);
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
            pic_attr.insert("Imagepath",filePath);
            pic_attr.insert("PicID",QString::fromUtf8(QCryptographicHash::hash(pic_file.readAll(),QCryptographicHash::Md5).toBase64()).left(32));
            pic_attr.insert("IsPredicted","0");

            success=database_manager_->insertRecord2Pic(pic_attr);
            qDebug()<<success;
            pic_file.close();
        }
        success = review_json_dataloader_->loadReviewJson(filePath);
        qDebug() << "review :::: " << success;
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
        QStringList pic_list=dir.entryList(QStringList()<<"*.jpg"<<"*.png",QDir::Files | QDir::NoDotAndDotDot);
        foreach (QString filename, pic_list) {
            success=loadPic(dir.filePath(filename));
        }
        QStringList dir_list=dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (QString dirname, dir_list) {
            success=loadPicsFromDir(dirPath + "/" + dirname);
        }
    }
    return success;
}
