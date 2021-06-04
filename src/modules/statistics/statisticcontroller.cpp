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

    // 属性过滤配置
    if (rootobj.contains("StatisticsFilter"))
    {
        statistics_filter_field_ = rootobj.value("StatisticsFilter").toObject();
    }

    // label表表头配置
    if (rootobj.contains("Area"))
    {
        statistics_label_dict_ = rootobj.value("Area").toObject().value("labelDict").toObject();
        initTableLabel();
        updateTableLabelData();
    }

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

void StatisticController::loadConfig(QString filename, QString tablename)
{
    loadConfig(filename);
}

void StatisticController::set_standard_4c2c(QString name)
{
    standard_4c2c_ = name;
}

void StatisticController::init()
{
    database_ = database_manager_->database();
    initTableLabel();
    updateTableLabelData();
    updatePieViewData();
    updateAttributeFilterData();
}

void StatisticController::initTableLabel()
{
    SqlRecordCountTableModel *currentModel = new SqlRecordCountTableModel(this);

    table_label_header_title_.clear();
    table_label_header_role_.clear();
    QVariantList   value;

    if (statistics_label_dict_.size() > 0) {
        QJsonArray labelArray =  statistics_label_dict_[0].toObject().value("options").toArray();
        for (int col_i = 0; col_i < labelArray.size(); col_i++)
        {
            QJsonArray label = labelArray[col_i].toArray();

            currentModel->insertColumn(col_i);
            currentModel->setColumn(col_i, label[0].toString());
            table_label_header_title_ << label[1].toString();
            table_label_header_role_ << label[0].toString();
            value << "0";
        }
    }
    else
    {
        QSqlQueryModel model;
        QStringList    result_columns;
        QString        condition;
        QString        tablename = "Standard";

        table_label_header_title_.clear();
        table_label_header_role_.clear();
        // "select LabelNum, LabelTitle from Standard where name = '2C'"
        result_columns << "LabelNum" << "LabelTitle";
        condition = "where name = '" + standard_4c2c_ + "'";
        database_manager_->getQueryResult(model, result_columns, tablename, QJsonArray(), condition);
        int totalCounts = model.rowCount();
        qDebug() << "totalCounts = " << totalCounts;

        if (totalCounts > 0) {
            QStringList titles = model.record(0).value(1).toString().split(";");
            label_title_num_ = titles.size();
            for (int col_i = 0; col_i < titles.size(); col_i++)
            {
                currentModel->insertColumn(col_i);
                currentModel->setColumn(col_i, QString::number(col_i));
                table_label_header_title_ << titles[col_i];
                table_label_header_role_ << QString::number(col_i);
                value << "0";
            }
        }
    }

    currentModel->insertRow(0);
    currentModel->setRow(0, QString::number(0), value);

    table_model_label_ = currentModel;
}

void StatisticController::updateTableLabelData()
{
    QVariantList   value;

    for (int col_i = 0; col_i < label_title_num_; col_i++)
    {
        value << "0";
    }

    // "select count(Predict_00) from Pic_2C where Predict_00 > 0"
    for (int col_i = 0; col_i < label_title_num_; col_i++) {
        QSqlQueryModel model;
        QString        columname = "Predict_" + QString::number(col_i).rightJustified(2,'0');
        QString        condition = "where " + columname + " > 0";
        QStringList    result_columns;
        result_columns << "count(" + columname + ")";

        database_manager_->getQueryResult(model, result_columns, "Pic_" + standard_4c2c_, user_filter_, condition);
        value[col_i] =  model.record(0).value(0).toString();
    }

    qDebug() << "model value = " << value;
    table_model_label_->setRow(0, QString::number(0), value);

}

void StatisticController::updatePieViewData()
{
    QSqlQueryModel model_bad;
    QSqlQueryModel model_ok;
    QStringList    result_columns;
    QString        condition;
    QString        tablename = "Pic_" + standard_4c2c_;

    // "select count(*) from Pic_2C where PredictResult = ReviewResult"
    result_columns << "count(*)";
    condition = "where PredictResult = 1";
    database_manager_->getQueryResult(model_ok, result_columns, tablename, user_filter_, condition);

    // "select count(*) from Pic_2C where PredictResult != ReviewResult"
    //condition = "where PredictResult != ReviewResult";
    condition = "where PredictResult = 2";
    database_manager_->getQueryResult(model_bad, result_columns, tablename, user_filter_, condition);

    pie_view_badnum_ = model_bad.record(0).value(0).toInt();
    pie_view_oknum_ =  model_ok.record(0).value(0).toInt();
}

void StatisticController::updateData()
{
    qDebug() << label_title_num_;
    if (label_title_num_ == 0) {
        initTableLabel();
    }

    updateTableLabelData();
    updatePieViewData();
    updateAttributeFilterData();
}

void StatisticController::updateUserFilter(QString attributeFilter,
                                           QString beginDate,
                                           QString endDate)
{
    user_attribute_filter_ = attributeFilter;
    user_begin_date_ = beginDate;
    user_end_date_ = endDate;

    QJsonArray userFilter;

    if (beginDate.size() > 0 && endDate.size() > 0)
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
    }

    if (user_attribute_filter_.size() > 0)
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

            user_attribute_filter_obj_ = condition_attr;
        }
    }

    user_filter_ = userFilter;
}

QList<int>StatisticController::getPieViewInfo()
{
    QList<int> info;

    info.push_back(pie_view_badnum_);
    info.push_back(pie_view_oknum_);

    return info;
}

SqlRecordCountTableModel * StatisticController::getTableLabelData()
{
    return table_model_label_;
}

QVariantList StatisticController::getTableLabelHeaderTitle()
{
    return table_label_header_title_;
}
QVariantList StatisticController::getTableLabelHeaderRole()
{
    return table_label_header_role_;
}

void StatisticController::updateAttributeFilterData()
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

    attribute_filter_jsonobj_ = result;
}

QJsonObject StatisticController::getAttributeFilterData() const
{
    return attribute_filter_jsonobj_;
}
