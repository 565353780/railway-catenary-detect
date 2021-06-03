#ifndef STATISTICCONTROLLER_H
#define STATISTICCONTROLLER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QtSql>
#include <QString>

class QAbstractTableModel;
class QSqlDatabase;
class QSqlTableModel;
class SqlRecordCountTableModel;
class RailwayCatenaryDataManager;

class StatisticController : public QObject {
    Q_OBJECT

public:

    explicit StatisticController(QObject                    *parent = nullptr,
                                 RailwayCatenaryDataManager *database_manager = nullptr);
    ~StatisticController();

    void loadConfig(QJsonObject rootobj);

    void loadConfig(QString filename);

public:

    Q_INVOKABLE void updateData();

    Q_INVOKABLE void updateUserFilter(QString attributeFilter,
                                      QString beginDate,
                                      QString endDate);

    /**
     *@return [picture_undetected_num_, picture_detected_num_]
     */
    Q_INVOKABLE QList<int>                getPictureDetectedInfo();

    Q_INVOKABLE SqlRecordCountTableModel* getTableLabelData();

    Q_INVOKABLE QJsonObject               getNameValuesForTimeFilter() const;

    Q_INVOKABLE QVariantList              getTableLabelHeader();

private:

    void init();

    void initTableLabel();

    void updateTableLabelData();

    void updatePiePictureDetectedData();

private:

    RailwayCatenaryDataManager *database_manager_ = nullptr;
    SqlRecordCountTableModel *table_model_label_ = nullptr;
    int picture_detected_num_;
    int picture_undetected_num_;
    QVariantList table_label_header_;

    QString user_attribute_filter_;
    QString user_begin_date_;
    QString user_end_date_;
    QJsonArray user_filter_;
    QJsonObject user_attribute_filter_obj_;

    QJsonObject statistics_filter_field_;
    QSqlDatabase *database_ = nullptr;

signals:

public slots:
};

#endif // STATISTICCONTROLLER_H
