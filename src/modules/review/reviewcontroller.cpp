#include "reviewcontroller.h"

#include <QFileInfo>
#include <QtSql>
#include <QTimer>
#include <QDebug>
#include<QMap>
#include <QSqlQueryModel>

#include"modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"

ReviewController::ReviewController(QObject *parent, RailwayCatenaryDataManager *database_manager,bool auto_load_predict_result_controller,bool is_predict)
    : QObject(parent),database_manager_(database_manager),auto_load_predict_result_controller_(auto_load_predict_result_controller),is_predict_(is_predict)
{
    loadConfig("./configs/review_config.json");
    loadLabelList("./configs/label_config_4c.json");//chLi
    object_detect_ = new ObjectDetect(this);
    pic_record_model_ = new QSqlTableModel(this,*database_manager_->database());
    predict_record_model_ =new QSqlTableModel(this,*database_manager_->database());
    review_record_model_ = new QSqlTableModel(this,*database_manager_->database());
    connect(object_detect_,SIGNAL(signalReceiveResult(QJsonObject)),this,SLOT(slotReceivePredictResult(QJsonObject)));
    set_pic_filter_array(1, 0);
    updatePicModel();
    current_image_index_= -1;
    review_image_show_status_ = 0;
    base_index_ = 0;
    predict_controller_ = false;
    image_current_info_map_.clear();
}

void ReviewController::updatePicModel()
{
    predict_current_index_ = 0;
    bool success = database_manager_->queryPicRecordInTable(pic_record_model_,pic_filter_array_);
    //qDebug()<<"update pic table success"<<success;
    image_count_total_ = pic_record_model_->rowCount();
    //qDebug()<<"image_count_total:"<<image_count_total_;
    initIsPredictMap();
}

void ReviewController::set_pic_filter_array(int isPredictedType, int predictResultType)
{
    qDebug() <<"isPredictedType = " << isPredictedType;
    qDebug() <<"predictResultType = " << predictResultType;

    QJsonArray filterArr;
    if (isPredictedType > 0) {
        QJsonObject obj;
        obj.insert("field", "IsPredicted");
        obj.insert("operator", "=");
        obj.insert("value", QString::number(isPredictedType - 1));
        filterArr.push_back(obj);
    }

    if (predictResultType > 0) {
        QJsonObject obj;
        obj.insert("field", "PredictResult");
        obj.insert("operator", "=");
        obj.insert("value", QString::number(predictResultType - 1));
        filterArr.push_back(obj);
    }

    pic_filter_array_ = filterArr;
}

void ReviewController::initIsPredictMap()
{
    unpredict_count_ = 0;
    for(int index = 0; index < image_count_total_; index++)
    {
        image_current_record_ = pic_record_model_->record(index);
        for(int i=0; i<image_current_record_.count(); ++i)
            image_current_info_map_[image_current_record_.fieldName(i)]=image_current_record_.value(i).toString();

        if(isInPredictDatabase())
            image_isPredict_.insert(image_current_info_map_.value("PicID"),1);
        else
        {
            unpredict_count_++;
            image_isPredict_.insert(image_current_info_map_.value("PicID"),0);
        }
    }
    predict_count_total_ = unpredict_count_;
    emit initInformation();
}

bool ReviewController::isInPredictDatabase()
{
    predict_filter_array_=QJsonArray();
    QJsonObject obj;
    obj.insert("field","PicID");
    obj.insert("operator","=");
    obj.insert("value",image_current_info_map_.value("PicID"));
    predict_filter_array_.push_back(obj);
    database_manager_->queryPredictRecordInTable(predict_record_model_,predict_filter_array_);
    int row_count=predict_record_model_->rowCount();
    if(row_count > 0)
        return true;
    else
        return false;
}

void ReviewController::setPredictController(bool target)
{
    predict_controller_ = target;
}

void ReviewController::setLoadPredictResultController(bool target)
{
    auto_load_predict_result_controller_ = target;
    updateImagePredictJsonArray();
}

void ReviewController::deleteMark(int ID)
{
    predict_result_array_.removeAt(ID);
    int length = predict_result_array_.size();
    for(int i = ID; i < length; i++)
    {
        QJsonObject value;
        QJsonObject temp = predict_result_array_.at(ID).toObject();
        predict_result_array_.removeAt(ID);
        temp.insert("ID",QString::number(i));
        value.insert("key",temp);
        predict_result_array_.push_back(value.value("key"));
    }
    qDebug()<<"predict_result_array:"<<predict_result_array_;
}

void ReviewController::reviseMark(int ID,int label)
{
    int length = predict_result_array_.size() - 1;
    QJsonObject value;

    QJsonObject temp = predict_result_array_.at(ID).toObject();
    predict_result_array_.removeAt(ID);
    temp.insert("Label",label);
    value.insert("key",temp);
    predict_result_array_.push_back(value.value("key"));

    for(int i = ID; i < length; i++)
    {
        temp = predict_result_array_.at(ID).toObject();
        predict_result_array_.removeAt(ID);
        value.insert("key",temp);
        predict_result_array_.push_back(value.value("key"));
    }
}

void ReviewController::addMark(QJsonObject mark)
{
    qDebug()<<"mark:" <<mark;
    mark.insert("Time",image_current_info_map_.value("Time"));
    mark.insert("PicID",image_current_info_map_.value("PicID"));
    mark.insert("ID",QString::number(predict_result_array_.size()));
    QJsonObject value;
    value.insert("key",mark);
    predict_result_array_.push_back(value.value("key"));
    qDebug()<<"add mark:"<<predict_result_array_;
}

void ReviewController::deleteCurrentMark()
{
    QString filter;
    filter = "PicID= '" + image_current_info_map_.value("PicID") + "'";
    database_manager_->removeRecordFromReview(filter);
}

void ReviewController::saveMarkToReview()
{
    deleteCurrentMark();
    QMap<QString,QString> result_map;
    for(int i = 0; i < predict_result_array_.size(); i++)
    {
        QJsonObject subArray =predict_result_array_[i].toObject();
        QStringList field_name_list_ = subArray.keys();
        foreach(auto key, field_name_list_){
            if(key == "x1" || key == "x2" || key == "y1" || key == "y2")
                result_map.insert(key,QString::number(subArray[key].toDouble(), 10, 10));
            else if(key == "Label"){
                result_map.insert(key,QString::number(subArray[key].toInt()));
            }
            else
                result_map.insert(key,subArray[key].toString());
        }
        database_manager_->insertRecord2Review(result_map);
    }

}

QList<QString> ReviewController::getLabelList()
{
    return label_list_;
}

void ReviewController::reviseCurrentImageInfo(QMap<QString,QString> attr_map)
{
    QMap<QString, QString> id_map;
    id_map["PicID"] = attr_map.value("PicID");
    attr_map["IsPredicted"] = QString::number(1);
    QStringList revise_list;
    revise_list.push_back("IsPredicted");
    revise_list.push_back("PredictResult");
    database_manager_->reviseRecordFromPic(id_map,attr_map,revise_list);
}

QString ReviewController::convertJsonArrrayToString(QJsonArray polygon)
{
    QString polygon_string;
    polygon_string.append("[");
    for(int i = 0; i < polygon.size(); i++)
    {
        polygon_string.append("[");
        polygon_string.append(QString::number(polygon[i].toArray()[0].toDouble(),10,10));
        polygon_string.append(",");
        polygon_string.append(QString::number(polygon[i].toArray()[1].toDouble(),10,10));
        polygon_string.append("]");
        if(i != polygon.size()-1)
            polygon_string.append(",");
    }
    polygon_string.append("]");
    return polygon_string;
}

QJsonArray ReviewController::convertStringToJsonArray(QString polygon_string)
{
    polygon_string.remove(0,1);
    polygon_string.remove(polygon_string.size()-1,1);

    for(int i = 0; i < polygon_string.size(); i++)
    {
        if(polygon_string[i] == '[' || polygon_string[i] == ']')
            polygon_string.remove(i,1);
    }

    QStringList polist = polygon_string.split(",");
    QJsonArray polygon = QJsonArray();
    QJsonArray temp = QJsonArray();
    int k = 0;
    for(int i = 0; i < polist.size(); i++)
    {
       temp.append(polist[i].toDouble());
       k++;
       if(k == 2)
       {
           polygon.append(temp);
           temp = QJsonArray();
           k = 0;
       }
    }
    return polygon;
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

void ReviewController::loadConfig(QString filename)
{
    QFile file(filename);
    if (! file.open(QIODevice::ReadOnly))
        qDebug()<<"open failed";
    QByteArray data = file.readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
    config_ = jsonDoc.object();
    if(config_.contains("firstDisplayFields"))
    {
        QJsonObject subArray = config_.value("firstDisplayFields").toObject();
        QStringList aoi_field_name_to_display_ = subArray.keys();
        foreach(auto key, aoi_field_name_to_display_){
            first_grid_display_field_name_.push_back(key);
            first_grid_display_field_title_.push_back(subArray[key].toString());
        }
    }
    if(config_.contains("secondDisplayFields"))
    {
        QJsonObject subArray = config_.value("secondDisplayFields").toObject();
        QStringList aoi_field_name_to_display_ = subArray.keys();
        foreach(auto key, aoi_field_name_to_display_){
            second_grid_display_field_name_.push_back(key);
            second_grid_display_field_title_.push_back(subArray[key].toString());
        }
    }
}

void ReviewController::loadLabelList(QString filename)
{
    QFile file(filename);
    if (! file.open(QIODevice::ReadOnly))
        qDebug()<<"open failed";
    QByteArray data = file.readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
    QJsonObject label_config = jsonDoc.object();

    if(label_config.contains("Area")){
        QJsonObject Area = label_config.value("Area").toObject();
        //qDebug()<<"Area:"<<Area;
        if(Area.contains("labelDict")){
            QJsonArray label_dict = Area.value("labelDict").toArray();
            QJsonObject temp = label_dict[0].toObject();
            QJsonArray option = temp.value("options").toArray();
            for(int i = 0; i < option.size(); i++)
                label_list_.push_back(option[i].toArray()[1].toString());
        }
    }
    qDebug()<<"label_list:"<<label_list_;
}

void ReviewController::startPredict()
{
    bool find_unpredict = true;
    if(unpredict_count_ > 0 && predict_controller_)
    {
        while(find_unpredict && predict_current_index_ <= image_count_total_)
        {
            updateImageInfo(predict_current_index_);
            predict_current_index_++;
            if(!isInPredictDatabase()){
                break;
            }
        }

        if(predict_current_index_ <= image_count_total_)
        {
            for(int i = 0; i < image_current_info_map_.size(); i++)
                current_pic_info_json_.insert(image_current_info_map_.keys()[i],image_current_info_map_.values()[i]);

            unpredict_count_--;
            //qDebug()<<"slot begin predict";
            object_detect_->slotBeginPredict(current_pic_info_json_);
        }
    }
}

int ReviewController::getUnpredictImageCount()
{
    return unpredict_count_;//pic IsPredicted 字段 count 1-*
}

int ReviewController::getNeedPredictCountTotal()
{
    return predict_count_total_;//pic IsPredicted 字段 count
}



void ReviewController::autoNextController()
{
    current_image_index_ = predict_current_index_ - 1;
    updateImageInfo(predict_current_index_-1);
    updateImagePath(predict_current_index_-1);
    updateImagePredictJsonArray();
    emit signalImageInfoUpdated();
}

QJsonArray ReviewController::getImagePredictInfo()
{
    //qDebug()<<"return array:"<<predict_result_array_;
    return predict_result_array_;
}

QJsonObject ReviewController::getPicJson()
{
    return current_pic_info_json_;
}

QJsonArray ReviewController::getFirstGridInfo()
{
    return first_grid_json_array_;
}

QJsonArray ReviewController::getSecondGridInfo()
{
    return second_grid_json_array_;
}

void ReviewController::nextImage()
{
    if(current_image_index_ < image_count_total_)
    {
        current_image_index_++;
        int index = current_image_index_;
        if(review_image_show_status_ == 1){
            index = is_predicted_image_map_.value(current_image_index_);
            if(current_image_index_ == image_count_total_)
                index = image_count_total_;
        }
        if(review_image_show_status_ == 2){
            index = not_predicted_image_map_.value(current_image_index_);
            if(current_image_index_ == image_count_total_)
                index = image_count_total_ + base_index_;
        }

        updateImageInfo(index);
        updateImagePath(index);

        updateImagePredictJsonArray();

        emit signalImageInfoUpdated();
    }
}

void ReviewController::previousImage()
{
    if(current_image_index_ >= 0)
    {
        current_image_index_--;
        int index = current_image_index_;
        if(review_image_show_status_ == 1){
            index = is_predicted_image_map_.value(current_image_index_);
            if(current_image_index_ == -1)
                index = -1;
        }
        if(review_image_show_status_ == 2){
            index = not_predicted_image_map_.value(current_image_index_);
            if(current_image_index_ == -1)
                index = -1;
        }
        updateImageInfo(index);
        updateImagePath(index);
        updateImagePredictJsonArray();
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

int ReviewController::getAccountOfCurrentInfoFromReview()
{
    return review_result_length_;
}

void ReviewController::updateImagePath(int index)
{
    if(-1 < index && index < (image_count_total_ + base_index_))
    {
        QString path= default_image_path_ + image_current_info_map_.value("Imagepath");
        QFileInfo info(path);
        if(info.exists()){
            current_image_path_="file:///"+ path;
        }
        else {
            current_image_path_="./resource/white";
        }
    }
    else if(current_image_index_ == -1){
        current_image_path_= "./resource/firstImage";
    }
    else {
        current_image_path_= "./resource/lastImage";
    }

}

void ReviewController::updateImagePredictJsonArray()
{
    predict_result_array_ = QJsonArray();

    if(is_predict_)
        updateImagePredictJsonArrayFromPredict();
    else
    {
        updateImagePredictJsonArrayFromReview();
        if(auto_load_predict_result_controller_)
            updateImagePredictJsonArrayFromPredict();
    }

    qDebug()<<"predict_result-array:"<<predict_result_array_;
}

void ReviewController::selectImageShowStatus(int status)
{
    current_image_index_ = -1;
    image_current_info_map_.clear();
    updateImageInfo(-1);
    updateImagePath(-1);
    updateImagePredictJsonArray();
    getIsPredictedAndNotPredictedImageMap();
    review_image_show_status_ = status;
    if(review_image_show_status_ == 0){
        base_index_ =0;
        image_count_total_ = pic_record_model_->rowCount();
    }
    if(review_image_show_status_ == 1){
        base_index_ = 0;
        image_count_total_ = is_predicted_image_map_.size();
    }
    if(review_image_show_status_ == 2){
        base_index_ = is_predicted_image_map_.size();
        image_count_total_ = not_predicted_image_map_.size();
    }
}



void ReviewController::updateImagePredictJsonArrayFromPredict()
{
    int ID = predict_result_array_.size();
    if(isInPredictDatabase())
    {
        int row_count=predict_record_model_->rowCount();
        int col_count=predict_record_model_->record().count();
        for(int i=0; i<row_count; ++i){
            QJsonObject obj;
            for(int j=0; j< col_count; ++j){
                if(predict_record_model_->record(i).fieldName(j) == "ID")
                    obj.insert("ID",QString::number(ID+i));
                else
                    obj.insert(predict_record_model_->record(i).fieldName(j),predict_record_model_->record(i).value(j).toJsonValue());
            }
            predict_result_array_.push_back(obj);
        }
    }
    else
        predict_result_array_=QJsonArray();
}

void ReviewController::updateImagePredictJsonArrayFromReview()
{
    review_filter_array_=QJsonArray();
    QJsonObject obj;
    obj.insert("field","PicID");
    obj.insert("operator","=");
    obj.insert("value",image_current_info_map_.value("PicID"));
    review_filter_array_.push_back(obj);
    database_manager_->queryReviewRecordInTable(review_record_model_,review_filter_array_);
    int row_count=review_record_model_->rowCount();
    int col_count=review_record_model_->record().count();
    review_result_length_ = row_count;
    if(row_count > 0)
    {
        for(int i=0; i<row_count; ++i){
            QJsonObject obj;
            for(int j=0; j< col_count; ++j){
                obj.insert(review_record_model_->record(i).fieldName(j),review_record_model_->record(i).value(j).toJsonValue());
            }
            predict_result_array_.push_back(obj);
        }
    }
}

QString ReviewController::getCurrentImagePath()
{
    return current_image_path_;
}



void ReviewController::updateImageInfo(int index)
{
    if(-1 < index && index < (image_count_total_ + base_index_) )
    {
        image_current_record_ = pic_record_model_->record(index);
        for(int i=0; i<image_current_record_.count(); ++i){
            image_current_info_map_[image_current_record_.fieldName(i)] = image_current_record_.value(i).toString();
        }
        if(isInPredictDatabase())
            image_current_info_map_["IsPredicted"] = QString::number(1);
        first_grid_json_array_ = QJsonArray();
        second_grid_json_array_ = QJsonArray();
        qDebug()<<"image_current_info_map:"<<image_current_info_map_;
        for(int i = 0 ; i  < first_grid_display_field_name_.size(); i++)
        {
            QJsonObject obj;
            obj.insert("title",first_grid_display_field_title_[i]);
            obj.insert("role",first_grid_display_field_name_[i]);
            obj.insert("value",image_current_info_map_.value(first_grid_display_field_name_[i]));
            first_grid_json_array_.push_back(obj);
        }

        for(int i = 0 ; i  < second_grid_display_field_name_.size(); i++)
        {
            QJsonObject obj;
            obj.insert("title",second_grid_display_field_title_[i]);
            obj.insert("role",0);
            obj.insert("value",image_current_info_map_.value(second_grid_display_field_name_[i]));
            second_grid_json_array_.push_back(obj);
        }
    }
    else
    {
        image_current_info_map_.clear();
    }
}

void ReviewController::getIsPredictedAndNotPredictedImageMap()
{
    int is_predicted_current_index = 0;
    int not_predicted_current_index = 0;
    for(int index = 0; index < image_count_total_; index++){
        image_current_record_ = pic_record_model_->record(index);
        for(int i=0; i<image_current_record_.count(); ++i){
            image_current_info_map_[image_current_record_.fieldName(i)] = image_current_record_.value(i).toString();
        }
        if(isInPredictDatabase()){
            is_predicted_image_map_.insert(is_predicted_current_index,index);
            is_predicted_current_index++;
        }
        else{
            not_predicted_image_map_.insert(not_predicted_current_index,index);
            not_predicted_current_index++;
        }
    }
}

void ReviewController::slotReceivePredictResult(QJsonObject result_obj)
{
    qDebug()<<"receive predict result";
    QJsonObject result = result_obj;
    QMap<QString,QString> pic_info_map;
    QMap<int,int> labelcount;
    QJsonArray predict_result_array = result.value("detections").toArray();
    unpredict_set_.remove(result.value("PicID").toString());
    for(int i = 0; i < predict_result_array.size(); i++)
    {
        QMap<QString,QString> result_map;

        result_map.insert("PicID",result.value("PicID").toString());
        QJsonObject subArray =predict_result_array[i].toObject();

        QStringList field_name_list_ = subArray.keys();
        foreach(auto key, field_name_list_){
            if(key == "type" || key == "Type")
                continue;
            if(key == "x1" || key == "x2" || key == "y1" || key == "y2")
                result_map.insert(key,QString::number(subArray[key].toDouble(), 10, 10));
            else if(key == "Label"){
                result_map.insert(key,QString::number(subArray[key].toInt()));
                labelcount[subArray[key].toInt()]=labelcount.value(subArray[key].toInt(),0)+1;
            }
            else
                result_map.insert(key,subArray[key].toString());
        }
        qDebug()<< "result_map:" << result_map;
        database_manager_->insertRecord2Predict(result_map);
    }
    pic_info_map = image_current_info_map_;
    if(predict_result_array.size()>0)
    {
        pic_info_map.insert("PredictResult",QString::number(2));

    }
    else{
        pic_info_map.insert("PredictResult",QString::number(1));
    }

    qDebug() <<"labelcount = " << labelcount;

    foreach( int label, labelcount) {
        pic_info_map.insert("Predict_"+QString::number(label).rightJustified(2,'0'), QString::number(labelcount.value(label)));
    }
    qDebug()<<"pic_info_map:"<<pic_info_map;
    reviseCurrentImageInfo(pic_info_map);
    qDebug()<<"finish predict";
    if(unpredict_count_ == predict_count_total_ - 1)
    {
        current_image_index_ = predict_current_index_ - 1;
        updateImageInfo(predict_current_index_-1);
        updateImagePath(predict_current_index_-1);
        updateImagePredictJsonArray();
        emit updateImagePredicitInfo();
    }

    emit currentImageFinished();

    if(unpredict_count_ > 0)
        startPredict();
}

void ReviewController::slotSetFilter(QJsonArray user_filter)
{
    pic_filter_array_=user_filter;
    updatePicModel();
}

