#ifndef REVIEWCONTROLLER_H
#define REVIEWCONTROLLER_H

#include <QObject>
#include <QSqlRecord>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include "modules/detect/objectdetect.h"

class QTimer;
class QSqlTableModel;
class QSqlQueryModel;
class USBRelayControll;
class SimpleClient;
class QSqlDatabase;
class AutoImporter;
class RailwayCatenaryDataManager;

class ReviewController : public QObject
{
    Q_OBJECT
public:
    explicit ReviewController(QObject *parent = nullptr, RailwayCatenaryDataManager* database_manager=nullptr);

    Q_INVOKABLE void updatePicModel();

    void updatePredictModel();

    //加入假数据
    void addPredictData();
    void addImagedata();





    //获取listview信息
    Q_INVOKABLE QJsonArray getImagePredictInfo();

    //从数据库获取当前图片的信息，保存为jsonObject,可供qml使用
    Q_INVOKABLE QJsonObject getPicJson();

    //获取GridView所需展示的信息
    Q_INVOKABLE QJsonArray getGridInfo();

    //下一张
    Q_INVOKABLE void nextImage();

    //上一张
    Q_INVOKABLE void previousImage();

    //获取图片总数
    Q_INVOKABLE QVariantList getCurrentIndexAndTotal();

    //更新当前图片地址
    Q_INVOKABLE void updateImagePath(int index);

    //获取当前图片地址
    Q_INVOKABLE QString getCurrentImagePath();

    //点击上一张或下一张后，在数据库查找当前图片信息，更新predict_result_array_,grid_json_array_,current_pic_info_json_
    void updateImageInfo(int index);

signals:

    //启动AI预测
     void sendPredicitRequest(QJsonObject current_pic_info_json);

    //AI预测完毕后更新listview信息
     void updateImagePredicitInfo();

    //点击上一张下一张后更新图片相关信息
     void signalImageInfoUpdated();

public slots:
    void slotReceivePredictResult(QJsonObject result_obj);
    //启动预测程序
    Q_INVOKABLE void startPredict();
private:

    int current_image_index_;
    int image_count_total_;
    int unpredict_count_;

    QString current_image_path_;
    QString default_image_path_;

    //存储当前image所有信息的map
    QMap<QString, QString>      image_current_info_map_;

    RailwayCatenaryDataManager* database_manager_;


    QSqlTableModel*             pic_record_model_;
    QSqlTableModel*             predict_record_model_;

    QSqlRecord              image_current_record_;
    QSqlRecord              predict_current_record_;

    ObjectDetect* object_detect_;


    QJsonArray pic_filter_array_;
    QJsonArray predict_filter_array_;

    QJsonArray predict_result_array_;
    QJsonArray grid_json_array_;
    QJsonObject current_pic_info_json_;

};

#endif // REVIEWCONTROLLER_H
