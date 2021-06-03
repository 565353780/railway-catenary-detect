#include "statisticcontroller.h"
#include "services/SqlTable/SqlRecordCountTableModel.h"
#include "modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"

StatisticController::StatisticController(QObject *parent, RailwayCatenaryDataManager *database_manager)
    : QObject(parent), database_manager_(database_manager)
{
    init();
}

StatisticController::~StatisticController()
{}

void StatisticController::loadConfig(QJsonObject rootobj)
{
    // do configuration

    if (rootobj.contains("StatisticsFilter"))
    {
        statistics_filter_field_ = rootobj.value("StatisticsFilter").toObject();
    }

    init();
}

void StatisticController::loadConfig(QString filename)
{
    QFile loadFile(filename);

    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "could't open statistics_config.json";
        return;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError json_error;
    QJsonDocument   jsonDoc(QJsonDocument::fromJson(allData, &json_error));

    if (json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "statistic_config json error!" << json_error.error;
        return;
    }

    loadConfig(jsonDoc.object());
}

void StatisticController::init()
{
    // do something
    database_ = new QSqlDatabase();

    initTableLabel();
    updatePiePictureDetectedData();
}

void StatisticController::initTableLabel()
{
    SqlRecordCountTableModel *currentModel = new SqlRecordCountTableModel(this);

    table_label_header_.clear();
    QVariantList   value;

    // label固定0-10
    for (int col_i = 0; col_i < 11; col_i++)
    {
        currentModel->insertColumn(col_i);
        currentModel->setColumn(col_i, QString::number(col_i));
        table_label_header_ << QString::number(col_i);
        value << "0";
    }
    currentModel->insertRow(0);
    qDebug() << "-----value------" << value;
    currentModel->setRow(0, QString::number(0), value);

    table_model_label_ = currentModel;
}

void StatisticController::updateTableLabelData()
{
    QVariantList   value;
    QSqlQueryModel model;
    QJsonObject    queryObj;
    QStringList    result_columns;

    // "select label, count(label) from Predict group by label;"
    result_columns << "label" << "count(label)";
    database_manager_->getQueryResult(model, result_columns, "Predict", "group by label;");
    int totalCounts = model.rowCount();
    int row_i = 0;

    for (int col_i = 0; col_i < 11; col_i++)
    {
        if (row_i < totalCounts && col_i == model.record(row_i).value(0).toInt()) {
            value << model.record(row_i++).value(1).toString();
        } else {
            value << "0";
        }
    }
    qDebug() << "-----value------" << value;
    table_model_label_->setRow(0, QString::number(0), value);

}

void StatisticController::updatePiePictureDetectedData()
{
    QSqlQueryModel model_undetected;
    QSqlQueryModel model_detected;
    QStringList    result_columns;
    QString condition;

    // "select count(*) from Review where Review.PicID in (select distinct PicID from Predict);"
    // "select 1% from 2% %3"
    result_columns << "count(*)";
    condition = "where Review.PicID in (select distinct PicID from Predict);";
    database_manager_->getQueryResult(model_detected, result_columns, "Review", condition);

    // "select count(*) from Review where Review.PicID not in (select PicID from Review);"
    condition = "where Review.PicID not in (select distinct PicID from Predict);";
    database_manager_->getQueryResult(model_undetected, result_columns, "Review", condition);

    picture_undetected_num_ = model_undetected.record(0).value(0).toInt();
    picture_detected_num_ = model_detected.record(0).value(0).toInt();

}

void StatisticController::updateData()
{
    updateTableLabelData();
    updatePiePictureDetectedData();
}

void StatisticController::updateUserFilter(QString attributeFilter,
                                           QString beginDate,
                                           QString endDate)
{
    user_attribute_filter_ = attributeFilter;
    user_begin_date_ = beginDate;
    user_end_date_ = endDate;

    QJsonArray userFilter;

    if ((QString::compare("", beginDate) != 0) && (QString::compare("", endDate) != 0))
    {
        QJsonObject condition_beginDate;
        QJsonObject condition_endDate;
        condition_beginDate.insert("key",      "Time");
        condition_beginDate.insert("operator", ">=");
        condition_beginDate.insert("value",    beginDate);
        condition_endDate.insert("key",      "Time");
        condition_endDate.insert("operator", "<=");
        condition_endDate.insert("value",    endDate);
        userFilter.push_back(condition_beginDate);
        userFilter.push_back(condition_endDate);

        QString datefilter = "( Time >= " + beginDate + " ) and ( " + "Time <= " + endDate + " )";
    }

    if ((QString::compare("", user_attribute_filter_) != 0))
    {
        QList<QString> list = attributeFilter.split(",");
        foreach(QString value, list)
        {
            QList<QString> kv = value.split(":");
            QJsonObject    condition_attr;

            condition_attr.insert("key",      kv[0]);
            condition_attr.insert("operator", "=");
            condition_attr.insert("value",    kv[1]);
            userFilter.push_back(condition_attr);

            QString attrFilter = " and ( " + kv[0] + " = " + kv[1] + " )";
            user_attribute_filter_obj_ = condition_attr;
        }
    }

    user_filter_ = userFilter;
}

QList<int>StatisticController::getPictureDetectedInfo()
{
    QList<int> pictureDetectedInfo;

    pictureDetectedInfo.push_back(picture_undetected_num_);
    pictureDetectedInfo.push_back(picture_detected_num_);

    return pictureDetectedInfo;
}

SqlRecordCountTableModel * StatisticController::getTableLabelData()
{
    return table_model_label_;
}

QVariantList StatisticController::getTableLabelHeader()
{
    return table_label_header_;
}

QJsonObject StatisticController::getNameValuesForTimeFilter() const
{
    QSqlQuery   query(*database_);
    QJsonObject result;

    foreach(QString field, statistics_filter_field_.keys())
    {
        QJsonObject fieldObj;

        fieldObj.insert("role",  field);
        fieldObj.insert("title", statistics_filter_field_.value(field));
        QString command = QString("SELECT DISTINCT %1 FROM AOIPart WHERE( (Time >= '%2') and (Time <= '%3') )").arg(field).arg(user_begin_date_).arg(user_end_date_);
        query.exec(command);
        QSqlRecord record = query.record();
        QJsonArray array;
        array.append(tr("ALL"));

        while (query.next()) {
            array.append(query.value(field).toJsonValue());
        }

        fieldObj.insert("values", array);
        result.insert(field, fieldObj);
    }

    return result;
}
