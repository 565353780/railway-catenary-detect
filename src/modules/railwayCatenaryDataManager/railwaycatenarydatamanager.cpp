#include "railwaycatenarydatamanager.h"

RailwayCatenaryDataManager::RailwayCatenaryDataManager(QObject *parent, QString database_type, QString connectionm_name, QString database_filename):
//  QObject(parent),database_type_(database_type),connectionm_name_(connectionm_name),database_filename_(database_filename)
    DatabaseManager(parent,database_type,connectionm_name,database_filename)
{
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

QList<QMap<QString, QString> > RailwayCatenaryDataManager::queryRecordFromPic(const QString &filter_str, int pic_type)
{
    QSqlTableModel *model = new QSqlTableModel(0,*DatabaseManager::database());
    if(pic_type == 0)
        model->setTable("Pic_2C");
    else if(pic_type == 1)
        model->setTable("Pic_4C");
    DatabaseManager::queryRecordInTable(model,filter_str);
    QList<QMap<QString,QString>> list;
    for(int i = 0; i < model->rowCount(); i++){
        QMap<QString,QString> map;
        map.insert("PicID",model->record(i).value("PicID").toString());
        map.insert("imagepath",model->record(i).value("imagepath").toString());
        map.insert("Time",model->record(i).value("Time").toString());
        map.insert("isPredicted",model->record(i).value("isPredicted").toString());
        list.push_back(map);
    }
    return list;
}

QList<QMap<QString,QString>> RailwayCatenaryDataManager::queryRecordFromPredict(const QString &filter_str)
{
    QSqlTableModel *model = new QSqlTableModel(0,*DatabaseManager::database());
    model->setTable("Predict");
    DatabaseManager::queryRecordInTable(model,filter_str);
    QList<QMap<QString,QString>> list;
    for(int i = 0; i < model->rowCount(); i++){
        QMap<QString,QString> map;
        map.insert("PicID",model->record(i).value("PicID").toString());
        map.insert("ID",model->record(i).value("ID").toString());
        map.insert("Time",model->record(i).value("Time").toString());
        map.insert("label",model->record(i).value("label").toString());
        map.insert("x1",model->record(i).value("x1").toString());
        map.insert("x2",model->record(i).value("x2").toString());
        map.insert("y1",model->record(i).value("y1").toString());
        map.insert("y2",model->record(i).value("y2").toString());
        list.push_back(map);
    }
    return list;
}

QList<QMap<QString, QString> > RailwayCatenaryDataManager::queryRecordFromReview(const QString &filter_str)
{
    QSqlTableModel *model = new QSqlTableModel(0,*DatabaseManager::database());
    model->setTable("Review");
    DatabaseManager::queryRecordInTable(model,filter_str);
    QList<QMap<QString,QString>> list;
    for(int i = 0; i < model->rowCount(); i++){
        QMap<QString,QString> map;
        map.insert("PicID",model->record(i).value("PicID").toString());
        map.insert("ID",model->record(i).value("ID").toString());
        map.insert("Time",model->record(i).value("Time").toString());
        map.insert("label",model->record(i).value("label").toString());
        map.insert("x1",model->record(i).value("x1").toString());
        map.insert("x2",model->record(i).value("x2").toString());
        map.insert("y1",model->record(i).value("y1").toString());
        map.insert("y2",model->record(i).value("y2").toString());
        list.push_back(map);
    }
    return list;
}

bool RailwayCatenaryDataManager::removeRecordFromPic(const QString &filter_str, int pic_type)
{
    QSqlTableModel *model = new QSqlTableModel(0,*DatabaseManager::database());
    if(pic_type == 0)
        model->setTable("Pic_2C");
    else if(pic_type == 1)
        model->setTable("Pic_4C");
    DatabaseManager::queryRecordInTable(model,filter_str);
    for(int i = 0; i < model->rowCount(); i++){
        model->removeRow(i);
    }
    return true;
}

bool RailwayCatenaryDataManager::removeRecordFromPredict(const QString &filter_str)
{
    QSqlTableModel *model = new QSqlTableModel(0,*DatabaseManager::database());
    model->setTable("Predict");
    DatabaseManager::queryRecordInTable(model,filter_str);
    for(int i = 0; i < model->rowCount(); i++){
        model->removeRow(i);
    }
    return true;
}

bool RailwayCatenaryDataManager::removeRecordFromReview(const QString &filter_str)
{
    QSqlTableModel *model = new QSqlTableModel(0,*DatabaseManager::database());
    model->setTable("Review");
    DatabaseManager::queryRecordInTable(model,filter_str);
    for(int i = 0; i < model->rowCount(); i++){
        model->removeRow(i);
    }
    return true;
}


bool RailwayCatenaryDataManager::queryPicRecordInTable(QSqlTableModel* model, QJsonArray &query_array, QStringList field_list, int pic_type)
{
    if(pic_type == 0)
        model->setTable("Pic_2C");
    else if(pic_type == 1)
        model->setTable("Pic_4C");
    QString command=(getQueryFilterFromJson(query_array));
    //qDebug()<<"command:"<<command;
    return queryRecordInTable(model,command);
}

bool RailwayCatenaryDataManager::queryPredictRecordInTable(QSqlTableModel *model, QJsonArray &query_array, QStringList field_list)
{
    model->setTable("Predict");
    QString command=(getQueryFilterFromJson(query_array));
    //qDebug()<<"command:"<<command;
    return queryRecordInTable(model,command);
}

bool RailwayCatenaryDataManager::queryReviewRecordInTable(QSqlTableModel *model, QJsonArray &query_array, QStringList field_list)
{
    model->setTable("Review");
    QString command=(getQueryFilterFromJson(query_array));
    //qDebug()<<"command:"<<command;
    return queryRecordInTable(model,command);
}

void RailwayCatenaryDataManager::getQueryResult(QSqlQueryModel& model, QStringList result_list, QString tablename, QJsonArray filter_array, QString custom_condition)
{

    QString command = QString("select %1 from %2 %3;")
            .arg(result_list.size() > 0 ? result_list.join(",") : "*")
            .arg(tablename)
            .arg(getFilterConditionString(filter_array, custom_condition));
    qDebug() << command;
    model.setQuery(command, *database());

    if (model.lastError().isValid())
        qDebug() << "get result fail:" << model.lastError();
}

QString RailwayCatenaryDataManager::getFilterConditionString(const QJsonArray &filter_array, QString custom_condition)
{
    QString conditon_cause = "";
    QStringList condition;

    for (int i = 0; i < filter_array.size(); ++i)
    {
        QJsonObject single_filter = filter_array.at(i).toObject();
        QString single_filter_str = QString("(%1 %2 '%3')")
                .arg(single_filter.value("key").toString())
                .arg(single_filter.value("operator").toString())
                .arg(single_filter.value("value").toString());
        condition << single_filter_str;
    }

    if (custom_condition.size() > 0)
    {
        if (condition.size() > 0) {
            if (custom_condition.contains("where")) {
                conditon_cause += custom_condition;
                conditon_cause += " and ";
                conditon_cause += condition.join(" and ");
            } else {
                conditon_cause += " where ";
                conditon_cause += condition.join(" and ");
                conditon_cause += custom_condition;
            }
        } else {
            conditon_cause += custom_condition;
        }
    }
    else if (condition.size() > 0)
    {
        conditon_cause += "where ";
        conditon_cause += condition.join(" and ");
    }

    return conditon_cause;
}

