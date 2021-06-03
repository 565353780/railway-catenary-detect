#ifndef SIMPLECLIENT_H
#define SIMPLECLIENT_H

#include<QObject>
#include <QQueue>
#include <QTimer>
#include <QJsonObject>
#include <QTcpSocket>

class SimpleClient : public QObject
{
    Q_OBJECT
public:
    SimpleClient(QObject* parent=0, int port=0);


public slots:
    Q_INVOKABLE void setUrl(QString);
    Q_INVOKABLE void setActive(bool);
    void sendTextMessage(QString);
    void slotSendObject(QJsonObject);
    Q_INVOKABLE void setAddress(QString);
    void slotReceiveMsg();
    void actualSendText();
signals:
    Q_INVOKABLE void currentStatus(int);
    Q_INVOKABLE void  receiveTextMsg(QString);
    void  clientReceiveTextMsg(QString);
private:
    int status_; //0:void 1:busy
    int port_;
    bool is_connected_=false;
    QString data_end_="#*#DataEnd#*#";
    QString address_;
    QString server_url_;
    QTcpSocket* client_socket_;
    QString msg_;
    QByteArray msg_array_;
    QTimer* check_status_timer_;
    QQueue<QString> write_queue_;
    QQueue<QString> read_queue_;

};

#endif // SIMPLECLIENT_H
