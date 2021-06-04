#include "reviewjsondataloader.h"

#include "modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"

ReviewJsonDataLoader::ReviewJsonDataLoader(RailwayCatenaryDataManager *database_manager, QObject *parent)
    :QObject(parent), database_manager_(database_manager)
{
    label_list_.clear();

    QFile json_file("configs/4C_U-shaped_config.json");

    if(json_file.open(QFile::ReadOnly))
    {
        QByteArray json_array = json_file.readAll();

        json_file.close();

        QJsonDocument json_doc = QJsonDocument::fromJson(json_array);

        QJsonObject json_Obj = json_doc.object();

        QJsonArray label_array = json_Obj.value("Area").toObject().value("labelDict").toArray()[0].toObject().value("options").toArray();

        for(int i = 0; i < label_array.size(); ++i)
        {
            label_list_.append(label_array[i].toArray()[0].toString());
        }
    }
}

bool ReviewJsonDataLoader::loadReviewJson(QString filename)
{
    QString abs_filename = filename;

    if(!dir_.isAbsolutePath(abs_filename))
    {
        abs_filename = dir_.absoluteFilePath(abs_filename);
    }

    if(!dir_.exists(abs_filename))
    {
        qDebug() << "ReviewJsonDataLoader::loadReviewJson : file not exist.";
        return false;
    }

    QString hash = getFileHash(abs_filename);

    QString file_path = abs_filename.left(abs_filename.lastIndexOf("/"));

    QString json_filename = file_path + "/json" + abs_filename.split(file_path)[1].split(".")[0] + ".json";

    QFile json_file(json_filename);

    if(!json_file.open(QFile::ReadOnly))
    {
        qDebug() << "ReviewJsonDataLoader::loadReviewJson : json file not exist.";
        return false;
    }

    QByteArray array = json_file.readAll();

    QJsonDocument json_doc;

    json_doc = json_doc.fromJson(array);

    QJsonObject json_Obj = json_doc.object();

    QJsonArray label_array = json_Obj.value("Area").toObject().value("labels").toArray();
    QJsonArray polygon_array = json_Obj.value("Area").toObject().value("polygons").toArray();
    QJsonArray ID_array = json_Obj.value("Area").toObject().value("track").toObject().value("Array").toArray();
    QJsonArray shape_array = json_Obj.value("Area").toObject().value("shape").toArray();

    int width = shape_array[0].toInt();
    int height = shape_array[1].toInt();

    if(ID_array.size() > 0)
    {
        for(int i = 0; i < ID_array.size(); ++i)
        {
            double x1 = 1.0 * polygon_array[i].toArray()[0].toArray()[0].toInt() / width;
            double y1 = 1.0 * polygon_array[i].toArray()[0].toArray()[1].toInt() / height;
            double x2 = 1.0 * polygon_array[i].toArray()[2].toArray()[0].toInt() / width;
            double y2 = 1.0 * polygon_array[i].toArray()[2].toArray()[1].toInt() / height;

            QJsonObject review_Obj;

            review_Obj.insert("PicID", hash);
            review_Obj.insert("ID", QString::number(ID_array[i].toInt()));
//            review_Obj.insert("Time", "Time Now");
            review_Obj.insert("Label", label_array[i].toArray()[0].toString().toInt());
            review_Obj.insert("x1", QString::number(x1));
            review_Obj.insert("x2", QString::number(x2));
            review_Obj.insert("y1", QString::number(y1));
            review_Obj.insert("y2", QString::number(y2));

            QJsonArray polygon_array;

            QJsonArray point1_array;
            point1_array.append(x1);
            point1_array.append(y1);
            polygon_array.append(point1_array);

            QJsonArray point2_array;
            point2_array.append(x1);
            point2_array.append(y2);
            polygon_array.append(point2_array);

            QJsonArray point3_array;
            point3_array.append(x2);
            point3_array.append(y2);
            polygon_array.append(point3_array);

            QJsonArray point4_array;
            point4_array.append(x2);
            point4_array.append(y1);
            polygon_array.append(point4_array);

            review_Obj.insert("Polygon", polygon_array);

            if(!insertReview(review_Obj))
            {
                qDebug() << "ReviewJsonDataLoader::loadReviewJson : insert review failed.";
                return false;
            }
        }
    }

    return true;
}

bool ReviewJsonDataLoader::insertReview(QString PicID,
                                        QString ID,
                                        QString Time,
                                        QString label,
                                        QString x1,
                                        QString x2,
                                        QString y1,
                                        QString y2,
                                        QString polygon)
{
    QMap<QString,QString> review_attr;

    review_attr.insert("PicID",PicID);
    review_attr.insert("ID",ID);
    review_attr.insert("Time",Time);
    review_attr.insert("Label",label);
    review_attr.insert("x1",x1);
    review_attr.insert("x2",x2);
    review_attr.insert("y1",y1);
    review_attr.insert("y2",y2);
    review_attr.insert("Polygon",polygon);

    if(!database_manager_->insertRecord2Review(review_attr))
    {
        qDebug() << "ReviewJsonDataLoader::insertPredictResult : insert failed.";
        return false;
    }

    return true;
}

bool ReviewJsonDataLoader::insertReview(QJsonObject insert_Obj)
{
    QString PicID = insert_Obj.value("PicID").toString();

    QString ID = "";
    if(insert_Obj.contains("ID"))
    {
        ID = insert_Obj.value("ID").toString();
    }

    QString Time = "";
    if(insert_Obj.contains("Time"))
    {
        Time = insert_Obj.value("Time").toString();
    }
    else
    {
        Time = QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss");
    }

    QString label = "";
    if(insert_Obj.contains("Label"))
    {
        label = insert_Obj.value("label").toString();
    }
    else
    {
        qDebug() << "ReviewJsonDataLoader::insertPredictResult : label not found.";
        return false;
    }

    QString x1 = "";
    if(insert_Obj.contains("x1"))
    {
        x1 = insert_Obj.value("x1").toString();
    }
    else
    {
        qDebug() << "ReviewJsonDataLoader::insertPredictResult : x1 not found.";
        return false;
    }

    QString x2 = "";
    if(insert_Obj.contains("x2"))
    {
        x2 = insert_Obj.value("x2").toString();
    }
    else
    {
        qDebug() << "ReviewJsonDataLoader::insertPredictResult : x2 not found.";
        return false;
    }

    QString y1 = "";
    if(insert_Obj.contains("y1"))
    {
        y1 = insert_Obj.value("y1").toString();
    }
    else
    {
        qDebug() << "ReviewJsonDataLoader::insertPredictResult : y1 not found.";
        return false;
    }

    QString y2 = "";
    if(insert_Obj.contains("y2"))
    {
        y2 = insert_Obj.value("y2").toString();
    }
    else
    {
        qDebug() << "ReviewJsonDataLoader::insertPredictResult : y2 not found.";
        return false;
    }

    QString polygon = "";
    if(insert_Obj.contains("Polygon"))
    {
        polygon = insert_Obj.value("polygon").toString();
    }
    else
    {
        qDebug() << "ReviewJsonDataLoader::insertPredictResult : polygon not found.";
        return false;
    }

    return insertReview(PicID,
                        ID,
                        Time,
                        label,
                        x1,
                        x2,
                        y1,
                        y2,
                        polygon);
}

QString ReviewJsonDataLoader::getFileHash(QString file_path)
{
    QString abs_file_path = file_path;

    if(!dir_.isAbsolutePath(abs_file_path))
    {
        abs_file_path = dir_.absoluteFilePath(file_path);
    }

    QFile file(abs_file_path);

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "DataManagerWidget::getFileHash : file open failed.";

        return "";
    }

    QCryptographicHash Hash(QCryptographicHash::Md5);

    QByteArray buf = file.readAll();
    Hash.addData(buf);

    file.close();

    QByteArray md5 = Hash.result().toBase64();
    return QString::fromUtf8(md5).left(32);
}
