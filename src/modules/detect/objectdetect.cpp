#include "objectdetect.h"

ObjectDetect::ObjectDetect(QObject *parent) : QObject(parent)
{
    detect_server_thread_ = new DetectServerThread(this);

    connect(detect_server_thread_, SIGNAL(signalBBoxOutput(QJsonObject)), this, SLOT(slotPushOutput(QJsonObject)));

    connect(detect_server_thread_, SIGNAL(signalBBoxOutput(QJsonObject)), this, SLOT(slotDetectQueue()));
}

void ObjectDetect::slotBeginPredict(QJsonObject img_Obj)
{
    img_Obj_queue_.enqueue(img_Obj);

    slotDetectQueue();
}

void ObjectDetect::slotDetectQueue()
{
    if(img_Obj_queue_.size() == 0 || detect_server_thread_->isRunning())
    {
        return;
    }

    detect_server_thread_->input(img_Obj_queue_.dequeue());

    detect_server_thread_->start();
}

void ObjectDetect::slotPushOutput(QJsonObject result_obj)
{
//    qDebug() <<"result:"<< result_obj;
    emit signalReceiveResult(result_obj);
}
