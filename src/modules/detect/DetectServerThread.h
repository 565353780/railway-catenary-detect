#ifndef DETECTSERVER_H
#define DETECTSERVER_H
#include <QDebug>
#include <QDir>
#include <QVector>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QThread>
#include <QRgb>
#include <QImage>
#include <QByteArray>

#include <ctime>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

#ifdef WIN32
#include "modules/detect/darknet/yolo_v2_class.hpp"
#endif

#ifdef Linux
#include "modules/detect/include/darknet.h"
#include <vector>
#include <string.h>
#endif

struct Info
{
    int img_width_{0};
    int img_height_{0};
    int img_format_{0};
};

struct ConnectedData
{
    Info info;

    QByteArray data_;

    inline int getRed(int i, int j)
    {
        return data_[3 *(j * info.img_width_ + i)];
    }
    inline int getGreen(int i, int j)
    {
        return data_[3 * (j * info.img_width_ + i) + 1];
    }

    inline int getBlue(int i, int j)
    {
        return data_[3 * (j * info.img_width_ + i) + 2];
    }

    inline void setPixel(int i,int j, int r, int g, int b)
    {
        data_[3 *(j * info.img_width_ + i)] = r;
        data_[3 *(j * info.img_width_ + i) + 1] = g;
        data_[3 *(j * info.img_width_ + i) + 2] = b;
    }

    void fromQImage(const QImage &img);
    void toQImage(QImage &img) const;
};

#ifdef Linux
    class DarknetDetector
    {
    private:
        std::vector<std::pair<char *, std::vector<float>>> detect(image im, float thresh=0.5, float hier_thresh=0.5, float nms=0.45);

        std::vector<std::pair<char *, std::vector<float>>> detect(char *img, float thresh=0.5, float hier_thresh=0.5, float nms=0.45);

        std::vector<std::pair<char *, std::vector<float>>> detect(float *img, int w, int h, int c, float thresh=0.5, float hier_thresh=0.5, float nms=0.45);

        network *net;
        metadata meta;

    public:
        DarknetDetector(const std::string &yolov3_cfg, const std::string &yolov3_weights, const std::string &coco_cfg);

        ~DarknetDetector();

        std::vector<std::pair<char *, std::vector<float>>> getDarknetResult(image img, float thresh=0.5, float hier_thresh=0.5, float nms=0.45);

        std::vector<std::pair<char *, std::vector<float>>> getDarknetResult(char *img, float thresh=0.5, float hier_thresh=0.5, float nms=0.45);

        std::vector<std::pair<char *, std::vector<float>>> getDarknetResult(float *img, int w, int h, int c, float thresh=0.5, float hier_thresh=0.5, float nms=0.45);
    };
#endif

class DetectServerThread :public QThread
{
    Q_OBJECT

public:
    DetectServerThread(QObject* parent=nullptr);
    ~DetectServerThread();

public:
#ifdef Linux
    QJsonObject darknet_process(QJsonObject img_Obj);
#endif

    QJsonObject darknet_test(QJsonObject img_Obj);

    bool input(QJsonObject img_Obj);

    void run();

signals:
    void signalBBoxOutput(QJsonObject result_obj);

private:
#ifdef Linux
    DarknetDetector* darknet_detector=NULL;
#endif

    QMutex mutex;
    QJsonObject img_Obj_;
    QJsonObject empty_output_Obj;

    QStringList label_list_;
};

#endif // DETECTSERVER_H
