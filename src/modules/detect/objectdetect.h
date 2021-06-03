#ifndef OBJECTDETECT_H
#define OBJECTDETECT_H

#include <QObject>
#include <QJsonObject>

#include "modules/detect/DetectServerThread.h"

class ObjectDetect : public QObject
{
    Q_OBJECT
public:
    explicit ObjectDetect(QObject *parent = nullptr);

signals:
    /**
     * @brief signalReceiveResult
     * @param result_obj            format:
     *                              {
     *                                  PicID:string, //MD5
     *                                  detections:[
     *                                      {
     *                                          label:string,
     *                                          type:"bbox"
     *                                          ID:string //50
     *                                          x1:float //0-1
     *                                          x2:float //0-1
     *                                          y1:float //0-1
     *                                          y2:float //0-1
     *
     *                                      }
     *                                  ]
     *                              }
     */
    void signalReceiveResult(QJsonObject result_obj);

public slots:
    void slotBeginPredict(QJsonObject img_Obj);

private slots:
    void slotDetectQueue();
    void slotPushOutput(QJsonObject result_obj);

private:
    DetectServerThread *detect_server_thread_;

    QQueue<QJsonObject> img_Obj_queue_;
};

#endif // OBJECTDETECT_H
