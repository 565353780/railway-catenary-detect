#include "MySocketClient.h"
#include <QDebug>
#include <QHostAddress>
#include <QJsonDocument>
MySocketClient::MySocketClient(QObject* parent, int port):
    QObject(parent),port_(port)
{
    client_socket_=new QTcpSocket(this);

    connect(client_socket_,&QTcpSocket::stateChanged,this,[=](QTcpSocket::SocketState state){
        emit currentStatus(int(state));
        qDebug()<<"client socket state:"<<state;
    });
    connect(client_socket_,&QTcpSocket::readyRead,this,&MySocketClient::slotReceiveMsg);
    check_status_timer_=new QTimer(this);
    check_status_timer_->setInterval(1000);
    connect(check_status_timer_,&QTimer::timeout, this, &MySocketClient::actualSendText);
}

void MySocketClient::setUrl(QString url)
{
    server_url_=url;
}

void MySocketClient::setActive(bool active)
{
    if(active){
        client_socket_->abort();
        bool success=client_socket_->bind(QHostAddress(address_));
        qDebug()<<"bind success"<<success<<"connecting:"<<server_url_<<":"<<port_;
        client_socket_->connectToHost(server_url_,port_);

    }
    else{
        client_socket_->disconnectFromHost();
    }
}

void MySocketClient::sendTextMessage(QString msg)
{
    //qDebug()<<client_socket_->isValid()<<client_socket_->isOpen();
    if(client_socket_->state()==QAbstractSocket::ConnectedState){
//        client_socket_->waitForBytesWritten();
//        client_socket_->write(msg.toUtf8());
//        client_socket_->waitForBytesWritten();
        write_queue_.push_back(msg);
        status_=1;
        check_status_timer_->start();
        actualSendText();
    }
}

void MySocketClient::sendTextMessage(QJsonObject obj)
{
    QJsonDocument doc(obj);
    sendTextMessage(QString::fromUtf8(doc.toJson()));
}

void MySocketClient::setAddress(QString address)
{
    address_=address;
}

void MySocketClient::slotReceiveMsg()
{
    QByteArray curData=client_socket_->readAll();
    QString curStringBack=(QString(curData.right(32)));
    msg_array_.append(curData);
    //qDebug()<<curStringBack;
    if(curStringBack.endsWith(data_end_)){
        msg_=QString::fromUtf8(msg_array_);

       //qDebug()<<msg_<<msg_.size();
        QStringList msglist=msg_.split(data_end_);
        foreach (QString msg, msglist) {
            emit receiveTextMsg(msg);
            emit clientReceiveTextMsg(msg);
        }
        //qDebug()<<"finish receive msg len:"<<msg_.size();
        msg_.clear();
        msg_array_.clear();
    }

}

void MySocketClient::actualSendText()
{

    if(write_queue_.size()>0){
        client_socket_->write(write_queue_.front().append(data_end_).toUtf8());
        client_socket_->waitForBytesWritten();
        write_queue_.pop_front();
    }
    else {
        status_=0;
        check_status_timer_->stop();
    }
}
