#ifndef REVIEWCONTROLLER_H
#define REVIEWCONTROLLER_H

#include <QObject>
#include <QSqlRecord>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QSet>
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
    explicit ReviewController(QObject *parent = nullptr, RailwayCatenaryDataManager* database_manager = nullptr,bool auto_load_predict_result_controller = true,bool is_predict = true);

    Q_INVOKABLE void updatePicModel();

    void updatePredictModel();

    //加载配置文件
    void loadConfig(QString filename);

    //加载labelList
    void loadLabelList(QString filename);

    //获取listview信息
    Q_INVOKABLE QJsonArray getImagePredictInfo();

    //从数据库获取当前图片的信息，保存为jsonObject,可供qml使用
    Q_INVOKABLE QJsonObject getPicJson();

    //获取第一栏GridView所需展示的信息
    Q_INVOKABLE QJsonArray getFirstGridInfo();

    //获取第二栏GridView所需展示信息
    Q_INVOKABLE QJsonArray getSecondGridInfo();

    //下一张
    Q_INVOKABLE void nextImage();

    //上一张
    Q_INVOKABLE void previousImage();

    //获取图片总数
    Q_INVOKABLE QVariantList getCurrentIndexAndTotal();

    //返回当前图片在review数据库里面的信息条数
    Q_INVOKABLE int getAccountOfCurrentInfoFromReview();

    //更新当前图片地址
    Q_INVOKABLE void updateImagePath(int index);

    //更新当前图片的预测信息
    void updateImagePredictJsonArray();

    //从predict数据库更新当前图片预测信息
    Q_INVOKABLE void updateImagePredictJsonArrayFromPredict();

    //从review数据库更新当前图片预测信息
    Q_INVOKABLE void updateImagePredictJsonArrayFromReview();

    //获取当前图片地址
    Q_INVOKABLE QString getCurrentImagePath();

    //点击上一张或下一张后，在数据库查找当前图片信息
    void updateImageInfo(int index);

    //选择复检页显示状态
    Q_INVOKABLE void selectImageShowStatus(int status);

    //对所有图片进行筛选
    Q_INVOKABLE void getIsPredictedAndNotPredictedImageMap();

    //启动预测程序
    Q_INVOKABLE void startPredict();

    //返回当前未预测的图片数目
    Q_INVOKABLE int getUnpredictImageCount();

    //返回需要预测的图片总数
    Q_INVOKABLE int getNeedPredictCountTotal();

    //当图片预测完成后，跳转到该图片
    Q_INVOKABLE void autoNextController();

    //读Pic数据库，读predict数据库，初始化当前数据库哪些图片要进行预测
    void initIsPredictMap();

    //判断照片ID在不在predict数据库数据库里面
    bool isInPredictDatabase();

    //设置startPredict开关
    Q_INVOKABLE void setPredictController(bool target);

    //设置review页面是否导入predict数据
    Q_INVOKABLE void setLoadPredictResultController(bool target);

    //删除当前Image其peidict内的mark
    Q_INVOKABLE void deleteMark(int ID);

    //更改当前mark的标签
    Q_INVOKABLE void reviseMark(int ID, int label);

    //给当前Image加入新的mark
    Q_INVOKABLE void addMark(QJsonObject mark);

    //删除review数据库内当前Image的mark信息
    void deleteCurrentMark();

    //将当前图片的mark写进review数据库
    Q_INVOKABLE void saveMarkToReview();

    //获取label_list
    Q_INVOKABLE QList<QString> getLabelList();

    //修改当前image状态为已预测
    Q_INVOKABLE void reviseCurrentImageInfo(QMap<QString, QString> attr_map);

    Q_INVOKABLE void set_pic_filter_array(int isPredictedType, int predictResultType);
    //将JsonArray转化成QString
    QString convertJsonArrrayToString(QJsonArray polygon);


    //将QString转化为QJsonArray
    QJsonArray convertStringToJsonArray(QString polygon_string);

signals:

    //启动AI预测
     void sendPredicitRequest(QJsonObject current_pic_info_json);

    //AI预测完毕后更新listview信息
     void updateImagePredicitInfo();

    //点击上一张下一张后更新图片相关信息
     void signalImageInfoUpdated();

    //加载图片后更新页面信息
    void  initInformation();

    //当前图片处理完毕
     void currentImageFinished();

public slots:
    void slotReceivePredictResult(QJsonObject result_obj);

    Q_INVOKABLE void slotSetFilter(QJsonArray user_filter);
private:

    int review_image_show_status_;

    int base_index_;

    QJsonObject config_;

    bool predict_controller_;

    bool is_predict_;

    bool auto_load_predict_result_controller_;

    int current_image_index_;
    int image_count_total_;
    int unpredict_count_;

    int review_result_length_;

    int predict_current_index_;
    int predict_count_total_;

    QSet<QString> unpredict_set_;//

    QString current_image_path_;
    QString default_image_path_;

    //存储当前image所有信息的map
    QMap<QString, QString>      image_current_info_map_;

    QMap<int,int> is_predicted_image_map_;
    QMap<int,int> not_predicted_image_map_;

    //存储图片是否已经被预测过
    QMap<QString,int>           image_isPredict_;

    QStringList first_grid_display_field_name_;

    QStringList first_grid_display_field_title_;

    QStringList second_grid_display_field_name_;

    QStringList second_grid_display_field_title_;

    RailwayCatenaryDataManager* database_manager_;


    QSqlTableModel*             pic_record_model_;
    QSqlTableModel*             predict_record_model_;
    QSqlTableModel*             review_record_model_;

    QList<QString> label_list_;

    QSqlRecord              image_current_record_;
    QSqlRecord              predict_current_record_;

    ObjectDetect* object_detect_;


    QJsonArray pic_filter_array_;
    QJsonArray predict_filter_array_;
    QJsonArray review_filter_array_;

    QJsonArray predict_result_array_;
    QJsonArray first_grid_json_array_;
    QJsonArray second_grid_json_array_;
    QJsonObject current_pic_info_json_;

};

#endif // REVIEWCONTROLLER_H
