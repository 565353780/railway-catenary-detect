#ifndef SIMPLESERVER_H
#define SIMPLESERVER_H

#include <QObject>
#include <QJsonObject>
class QTcpServer;
class QTcpSocket;

class SimpleServer : public QObject
{
    Q_OBJECT
public:
    explicit SimpleServer(QObject *parent = nullptr,int port=0);

signals:
    void requestReceived(QString method, QJsonObject content);
public slots:
    void slotReadRequest();
private:
    int port_;
    QTcpServer* server_;
    QTcpSocket* read_request_socket_;
};

class HandleCLientServer: public QObject{
    Q_OBJECT
public:
    explicit HandleCLientServer(QObject *parent = nullptr,int port=0);

signals:
    Q_INVOKABLE void requestReceived(QString method, QJsonObject content);
    Q_INVOKABLE void receiveText(QString);
    Q_INVOKABLE void receiveJson(QJsonObject);
    void signalReceiveText(QString);
public slots:
    void slotReadRequest();
    Q_INVOKABLE void sendObject(QJsonObject);
    Q_INVOKABLE void sendTextMessage(QString);
private:
    int port_;
    QTcpServer* server_;
    QTcpSocket* client_socket_=nullptr;
    QString  content_;
    QByteArray content_data_;
};

#endif // SIMPLESERVER_H
