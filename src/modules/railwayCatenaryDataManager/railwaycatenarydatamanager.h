#ifndef RAILWAYCATENARYDATAMANAGER_H
#define RAILWAYCATENARYDATAMANAGER_H

#include <QObject>
#include <QFile>
#include <QJsonParseError>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "services/DatabaseManager/databasemanager.h"

class DatabaseManager;
class RailwayCatenaryDataManager : public DatabaseManager
{
    Q_OBJECT
private:
//    QSqlDatabase database_;
//    QSqlQuery*  query_;
//    QStringList table_name_list_;
//    QString database_type_;
//    QString connectionm_name_;
//    QString database_filename_;
    QStringList path = QStringList()<<"DatabaseConfig"<<"TableList";
protected:
    QJsonObject root_config_obj_;
public:
    explicit RailwayCatenaryDataManager(QObject* parent=nullptr,QString database_type="QSQLITE",
                                        QString connectionm_name="maindb", QString database_filename="mainDb.db");
    Q_INVOKABLE void loadConfig(QString filename);

//    void initDatabaseFromConfig(QJsonObject root_config_obj_);
//    bool insertRecord(const QString &table_name, const QMap<QString,QString> &attr_map);

    void set_root_config_obj(QJsonObject obj){
        root_config_obj_=obj;
    }
    QJsonObject get_root_config_obj_(){
        return root_config_obj_;
    }

    void setValue(QStringList path, QVariant value);
    QVariant getValue(QStringList path);

    QJsonArray getTableListArray();

    bool insertRecord2Pic(const QMap<QString,QString> &attr_map){
        return DatabaseManager::insertRecord("Pic",attr_map);
    }
    bool insertRecord2Predict(const QMap<QString,QString> &attr_map){
        return DatabaseManager::insertRecord("Predict",attr_map);
    }
    bool insertRecord2Review(const QMap<QString,QString> &attr_map){
        return DatabaseManager::insertRecord("Review",attr_map);
    }

    bool reviseRecordFromPic(const QMap<QString, QString> &id_map,
                                       const QMap<QString, QString> &record_map, QStringList revise_list){
        return DatabaseManager::reviseRecord("Pic",id_map,record_map,revise_list);
    }
    bool reviseRecordFromPredict(const QMap<QString, QString> &id_map,
                                       const QMap<QString, QString> &record_map, QStringList revise_list){
        return DatabaseManager::reviseRecord("Predict",id_map,record_map,revise_list);
    }
    bool reviseRecordFromReview(const QMap<QString, QString> &id_map,
                                       const QMap<QString, QString> &record_map, QStringList revise_list){
        return DatabaseManager::reviseRecord("Review",id_map,record_map,revise_list);
    }


    bool queryPicRecordInTable(QSqlTableModel *model, QJsonArray &query_array , QStringList field_list=QStringList());

    bool queryPredictRecordInTable(QSqlTableModel *model, QJsonArray &query_array , QStringList field_list=QStringList());

    bool queryReviewRecordInTable(QSqlTableModel *model, QJsonArray &query_array , QStringList field_list=QStringList());

    void getQueryResult(QSqlQueryModel& model, QStringList result_list, QString tablename, QString condition);


signals:

public slots:
};

#endif // RAILWAYCATENARYDATAMANAGER_H
