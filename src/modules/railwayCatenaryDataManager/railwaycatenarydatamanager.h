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
    QStringList path = QStringList()<<"DatabaseConfig"<<"TableList";
protected:
    QJsonObject root_config_obj_;
public:
    explicit RailwayCatenaryDataManager(QObject* parent=nullptr,QString database_type="QSQLITE",
                                        QString connectionm_name="maindb", QString database_filename="mainDb.db");
    Q_INVOKABLE void loadConfig(QString filename);

    void set_root_config_obj(QJsonObject obj){
        root_config_obj_=obj;
    }
    QJsonObject get_root_config_obj_(){
        return root_config_obj_;
    }

    void setValue(QStringList path, QVariant value);
    QVariant getValue(QStringList path);

    QJsonArray getTableListArray();

    bool insertRecord2Pic(const QMap<QString,QString> &attr_map, int pic_type=0){
        if(pic_type == 0)
            return DatabaseManager::insertRecord("Pic_2C",attr_map);
        else if(pic_type == 1)
            return DatabaseManager::insertRecord("Pic_4C",attr_map);
    }
    bool insertRecord2Predict(const QMap<QString,QString> &attr_map){
        return DatabaseManager::insertRecord("Predict",attr_map);
    }
    bool insertRecord2Review(const QMap<QString,QString> &attr_map){
        return DatabaseManager::insertRecord("Review",attr_map);
    }

    bool reviseRecordFromPic(const QMap<QString, QString> &id_map,
                                       const QMap<QString, QString> &record_map, QStringList revise_list, int pic_type=0){
        if(pic_type == 0)
            return DatabaseManager::reviseRecord("Pic_2C",id_map,record_map,revise_list);
        else if(pic_type == 1)
            return DatabaseManager::reviseRecord("Pic_4C",id_map,record_map,revise_list);
    }
    bool reviseRecordFromPredict(const QMap<QString, QString> &id_map,
                                       const QMap<QString, QString> &record_map, QStringList revise_list){
        return DatabaseManager::reviseRecord("Predict",id_map,record_map,revise_list);
    }

    bool reviseRecordFromReview(const QMap<QString, QString> &id_map,
                                       const QMap<QString, QString> &record_map, QStringList revise_list){
        return DatabaseManager::reviseRecord("Review",id_map,record_map,revise_list);
    }

    QList<QMap<QString,QString>> queryRecordFromPic(const QString &filter_str, int pic_type=0);
    QList<QMap<QString,QString>> queryRecordFromPredict(const QString &filter_str);
    QList<QMap<QString,QString>> queryRecordFromReview(const QString &filter_str);

    bool removeRecordFromPic(const QString &filter_str, int pic_type=0);
    bool removeRecordFromPredict(const QString &filter_str);
    bool removeRecordFromReview(const QString &filter_str);

    bool queryPicRecordInTable(QSqlTableModel *model, QJsonArray &query_array , QStringList field_list=QStringList(), int pic_type=0);

    bool queryPredictRecordInTable(QSqlTableModel *model, QJsonArray &query_array , QStringList field_list=QStringList());

    bool queryReviewRecordInTable(QSqlTableModel *model, QJsonArray &query_array , QStringList field_list=QStringList());

    void getQueryResult(QSqlQueryModel& model, QStringList result_list, QString tablename, QJsonArray filter_array, QString custom_condition = "");

    QString getFilterConditionString(const QJsonArray &filter_array, QString custom_condition = "");

signals:

public slots:
};

#endif // RAILWAYCATENARYDATAMANAGER_H
