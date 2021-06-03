#include "railwaycatenarydatamanager.h"

RailwayCatenaryDataManager::RailwayCatenaryDataManager(QObject *parent, QString database_type, QString connectionm_name, QString database_filename):
//  QObject(parent),database_type_(database_type),connectionm_name_(connectionm_name),database_filename_(database_filename)
    DatabaseManager(parent,database_type,connectionm_name,database_filename)
{
//    database_=QSqlDatabase::addDatabase(database_type_,connectionm_name_);
//    database_.setDatabaseName(database_filename_);
//    bool success=database_.open();
//    qDebug()<<"database open success?:"<<success;
//    query_=new QSqlQuery(database_);
}

void RailwayCatenaryDataManager::loadConfig(QString filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);

    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(file.readAll(),&err);

    if(err.error!=QJsonParseError::NoError){
        qDebug()<<"parse error:"<<err.errorString();
        return;
    }

    set_root_config_obj(doc.object());
    QJsonObject root_config_json_ = get_root_config_obj_();

    if(root_config_json_.contains("DatabaseConfig")){
        qDebug()<<"enter initDatabaseFromConfig";
//        initDatabaseFromConfig(root_config_json_.value("DatabaseConfig").toObject());
        DatabaseManager::loadConfig(root_config_json_.value("DatabaseConfig").toObject());
    }
}

void RailwayCatenaryDataManager::setValue(QStringList path, QVariant value)
{
    QList<QJsonObject> mid_obj_list;
    qDebug()<<"before revise"<<getValue(path);
    QJsonObject current_obj=root_config_obj_;

    int num=path.size();

    //
    for (int i=0; i<num-1; ++i) {
        QString key=path.at(i);
        if(current_obj.contains(key)){
            if(current_obj[key].isObject()){
                current_obj=current_obj[key].toObject();
                mid_obj_list.push_back(current_obj);
            }
            else {
                qDebug()<<"key error"<<key;
                return;
            }
        }
        else {

            return;
        }
    }

    if(current_obj.contains(path.back()) ){
        mid_obj_list.back()[path.back()]=QJsonValue::fromVariant(value);
    }
    else{
        qDebug()<<"last key error:"<<path.back();
        return;
    }

    for(int i=num-3; i>-1; --i){
        mid_obj_list[i][path[i+1]]=mid_obj_list[i+1];
    }
    root_config_obj_[path[0]]=mid_obj_list[0];

    qDebug()<<"after revise"<<getValue(path);
}

QVariant RailwayCatenaryDataManager::getValue(QStringList path)
{
    QJsonObject current_obj=root_config_obj_;
    int num=path.size();
    for (int i=0; i<num-1; ++i) {
        QString key=path.at(i);
        if(current_obj.contains(key)){
            if(current_obj[key].isObject())
                current_obj=current_obj[key].toObject();
            else {
                qDebug()<<"key error"<<key;
                return QVariant();
            }
        }
        else {
            return QVariant();
        }
    }
    if(current_obj.contains(path.back()) ){
        return current_obj[path.back()].toVariant();
    }
}

QJsonArray RailwayCatenaryDataManager::getTableListArray()
{
    QJsonObject current_obj=get_root_config_obj_();
    QJsonArray current_array;
    int num=path.size();
    for (int i=0; i<num; ++i) {
        QString key=path.at(i);
        if(current_obj.contains(key)){
            if(current_obj[key].isObject()){
                current_obj=current_obj[key].toObject();
            }
            else if (current_obj[key].isArray()) {
                current_array = current_obj[key].toArray();
                break;
            }
            else {
                qDebug()<<"key error"<<key;
                return QJsonArray();
            }
        }
        else {
            return QJsonArray();
        }
    }
    return current_array;
}

bool RailwayCatenaryDataManager::queryPicRecordInTable(QSqlTableModel* model, QJsonArray &query_array, QStringList field_list)
{
    model->setTable("Pic");
    QString command=(getQueryFilterFromJson(query_array));
    qDebug()<<"command:"<<command;
    return queryRecordInTable(model,command);
}

bool RailwayCatenaryDataManager::queryPredictRecordInTable(QSqlTableModel *model, QJsonArray &query_array, QStringList field_list)
{
    model->setTable("Predict");
    QString command=(getQueryFilterFromJson(query_array));
    qDebug()<<"command:"<<command;
    return queryRecordInTable(model,command);
}

bool RailwayCatenaryDataManager::queryReviewRecordInTable(QSqlTableModel *model, QJsonArray &query_array, QStringList field_list)
{
    model->setTable("Review");
    QString command=(getQueryFilterFromJson(query_array));
    qDebug()<<"command:"<<command;
    return queryRecordInTable(model,command);
}
void RailwayCatenaryDataManager::getQueryResult(QSqlQueryModel& model, QStringList result_list, QString tablename, QString condition)
{

    QString command = QString("select %1 from %2 %3").arg(result_list.size() > 0 ? result_list.join(",") : "*").arg(tablename).arg(condition);

    model.setQuery(command, *database());

    if (model.lastError().isValid())
        qDebug() << "get result fail:" << model.lastError();
}
