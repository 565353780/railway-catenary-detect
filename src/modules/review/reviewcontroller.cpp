#include "reviewcontroller.h"

#include <QFileInfo>
#include <QtSql>
#include <QTimer>
#include <QDebug>
#include<QMap>
#include <QSqlQueryModel>

#include"modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"

ReviewController::ReviewController(QObject *parent, RailwayCatenaryDataManager *database_manager)
    : QObject(parent),database_manager_(database_manager)
{
    object_detect_ = new ObjectDetect(this);
    //addPredictData();
    //addImagedata();
    pic_record_model_=new QSqlTableModel(this,*database_manager_->database());
    //pic_record_model_->setTable("Pic");
    predict_record_model_=new QSqlTableModel(this,*database_manager_->database());
    //predict_record_model_->setTable("Predict");
    connect(object_detect_,SIGNAL(signalReceiveResult(QJsonObject)),this,SLOT(slotReceivePredictResult(QJsonObject)));

    updatePicModel();
    updatePredictModel();
    current_image_index_=0;
}

void ReviewController::updatePicModel()
{
    pic_filter_array_=QJsonArray();
    QJsonObject obj;
    obj.insert("field","isPredicted");
    obj.insert("operator","=");
    obj.insert("value","0");
    pic_filter_array_.push_back(obj);
    bool success=database_manager_->queryPicRecordInTable(pic_record_model_,pic_filter_array_);
    qDebug()<<"update pic table success"<<success;
    image_count_total_=pic_record_model_->rowCount();
    qDebug()<<image_count_total_;
}

void ReviewController::updatePredictModel()
{
    predict_filter_array_=QJsonArray();
    QJsonObject obj;
    obj.insert("field","PicID");
    obj.insert("operator","=");
    obj.insert("value",current_pic_info_json_["PicID"].toString());

    predict_filter_array_.push_back(obj);
    database_manager_->queryPredictRecordInTable(predict_record_model_,predict_filter_array_);
    int row_count=predict_record_model_->rowCount();
    int col_count=predict_record_model_->record().count();
    predict_result_array_=QJsonArray();
    for(int i=0; i<row_count; ++i){
        QJsonObject obj;
        for(int j=0; j< col_count; ++j){
            obj.insert(predict_record_model_->record(i).fieldName(j),predict_record_model_->record(i).value(j).toJsonValue());
        }
        predict_result_array_.push_back(obj);
    }
}

void ReviewController::addPredictData()
{
    QJsonObject key;
    QJsonObject *model_information;
    for(int i = 0; i < 8; i++)
    {
        model_information = new QJsonObject();
        model_information->insert("PicTD",QString::number(i));
        model_information->insert("label",QString::number(i));
        model_information->insert("ID",QString::number(i));
        model_information->insert("x1",QString::number(i));
        model_information->insert("x2",QString::number(i));
        model_information->insert("y1",QString::number(i));
        model_information->insert("y2",QString::number(i));
        key.insert("key",*model_information);
        predict_result_array_.append(key.value("key"));
        delete model_information;
    }
    //qDebug()<<predict_result_array_;
}

void ReviewController::addImagedata()
{
    QJsonObject key;
    QJsonObject *model_information;
    QStringList title;
    QStringList value;

    title.push_back("Time");
    title.push_back("ImagePath");
    title.push_back("isPredicted");

    value.push_back("2020 08/06 14:22");
    value.push_back("C:\\Users\\ABC3D\\Desktop\\task\\1.jpg");
    value.push_back("NOT");

    for(int i = 0; i < title.size(); i++)
    {
        model_information = new QJsonObject();
        model_information->insert("title",title[i]);
        model_information->insert("value",value[i]);
        model_information->insert("role",0);
        key.insert("key",*model_information);
        grid_json_array_.append(key.value("key"));
        delete model_information;
    }

    //qDebug()<<grid_json_array_;
}

void ReviewController::startPredict()
{
    object_detect_->slotBeginPredict(current_pic_info_json_);
//    QJsonObject result = object_detect.getResult();
//    qDebug()<<"result json obj" << result;
//    qDebug()<<object_detect.getResult();
}


QJsonArray ReviewController::getImagePredictInfo()
{
    return predict_result_array_;
}

QJsonObject ReviewController::getPicJson()
{
    return current_pic_info_json_;
}

QJsonArray ReviewController::getGridInfo()
{
    return grid_json_array_;
}

void ReviewController::nextImage()
{
    if(current_image_index_ < image_count_total_)
    {
        current_image_index_++;

        updateImageInfo(current_image_index_);
        updateImagePath(current_image_index_);
        emit signalImageInfoUpdated();
    }
}

void ReviewController::previousImage()
{
    if(current_image_index_ >= 0)
    {
        current_image_index_--;
        updateImageInfo(current_image_index_);
        updateImagePath(current_image_index_);
        emit signalImageInfoUpdated();
    }
}

QVariantList ReviewController::getCurrentIndexAndTotal()
{
    QVariantList result;
    result<<current_image_index_+1;
    result<<image_count_total_;
    return result;
}

void ReviewController::updateImagePath(int index)
{
    if(-1 < index && index < image_count_total_)
    {
        QString path= default_image_path_ + image_current_info_map_.value("imagepath");
        path=path.split(";").at(0);
        QFileInfo info(path);
        //qDebug()<<"image path"<<path<<"info"<<image_current_info_map_;
        if(info.exists()){
            current_image_path_="file:///"+ path;
        }
        else {
            current_image_path_="/white";
        }
    }
    else if(current_image_index_ == -1){
        current_image_path_= "/firstImage";

    }
    else {
        current_image_path_= "/lastImage";
    }
}

QString ReviewController::getCurrentImagePath()
{
    return current_image_path_;
}

void ReviewController::updateImageInfo(int index)
{
    if(-1 < index && index < image_count_total_)
    {
        image_current_record_ = pic_record_model_->record(index);
        //qDebug()<<"record "<<image_current_record_<<index;
        grid_json_array_=QJsonArray();
        QStringList displaylist;
        displaylist<<"Time"<<"imagepath";
        for(int i=0; i<image_current_record_.count(); ++i){
            image_current_info_map_[image_current_record_.fieldName(i)]=image_current_record_.value(i).toString();
            current_pic_info_json_[image_current_record_.fieldName(i)]=image_current_record_.value(i).toString();
            QJsonObject obj;
            //if(image_current_record_.fieldName(i))
            obj.insert("title",image_current_record_.fieldName(i));
            obj.insert("role",image_current_record_.fieldName(i));
            obj.insert("value",image_current_record_.value(i).toJsonValue());
            grid_json_array_.push_back(obj);
        }

        updatePredictModel();
    }
    else
    {
        image_current_info_map_.clear();
    }
}

void ReviewController::slotReceivePredictResult(QJsonObject result_obj)
{
    qDebug()<<"result";
    QJsonObject result=result_obj;// = object_detect_->getResult();
    qDebug()<<"result json obj" << result;
    predict_result_array_ = result.value("detections").toArray();
    qDebug()<<predict_result_array_;
    emit updateImagePredicitInfo();
}

