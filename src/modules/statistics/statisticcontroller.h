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

    void loadConfig(QString filename, QString tablename);

    void set_standard_4c2c(QString name);

public:

    Q_INVOKABLE void updateData();

    Q_INVOKABLE void updateUserFilter(QString attributeFilter,
                                      QString beginDate,
                                      QString endDate);

    /**
     *@return [pie_view_badnum_, pie_view_oknum_]
     */
    Q_INVOKABLE QList<int>                getPieViewInfo();

    Q_INVOKABLE SqlRecordCountTableModel* getTableLabelData();

    Q_INVOKABLE QJsonObject               getAttributeFilterData() const;

    Q_INVOKABLE QVariantList              getTableLabelHeaderTitle();

    Q_INVOKABLE QVariantList              getTableLabelHeaderRole();

private:

    void init();

    void initTableLabel();

    void updateTableLabelData();

    void updatePieViewData();

    void updateAttributeFilterData();

private:

    RailwayCatenaryDataManager *database_manager_ = nullptr;
    SqlRecordCountTableModel *table_model_label_ = nullptr;
    int pie_view_oknum_;
    int pie_view_badnum_;
    QVariantList table_label_header_title_; // 显示内容
    QVariantList table_label_header_role_;

    QString standard_4c2c_ = "2C";
    int label_title_num_ = 0;

    QString user_attribute_filter_;
    QString user_begin_date_;
    QString user_end_date_;
    QJsonArray user_filter_;
    QJsonObject user_attribute_filter_obj_;

    QJsonObject statistics_filter_field_;
    QJsonObject statistics_label_dict_;
    QSqlDatabase *database_ = nullptr;

    QJsonObject attribute_filter_jsonobj_;

signals:

public slots:
};

#endif // STATISTICCONTROLLER_H
