#include "SimpleClient.h"
#include <QDebug>
#include <QHostAddress>
#include <QJsonDocument>

SimpleClient::SimpleClient(QObject* parent, int port):
    QObject(parent),port_(port)
{
    client_socket_=new QTcpSocket(this);

    connect(client_socket_,&QTcpSocket::stateChanged,this,[=](QTcpSocket::SocketState state){
        emit currentStatus(int(state));
        if(state==QTcpSocket::SocketState::ConnectedState){
            qDebug()<<"client socket state:"<<state;
            is_connected_=true;
            emit currentStatus(int(state));
        }
        else if(is_connected_==true && state==QTcpSocket::SocketState::UnconnectedState){
            is_connected_=false;
            qDebug()<<"client socket state:"<<state;
            emit currentStatus(int(state));
        }
    });
    connect(client_socket_,&QTcpSocket::readyRead,this,&SimpleClient::slotReceiveMsg);
    check_status_timer_=new QTimer(this);
    check_status_timer_->setInterval(1000);
    connect(check_status_timer_,&QTimer::timeout, this, &SimpleClient::actualSendText);
}

void SimpleClient::setUrl(QString url)
{
    server_url_=url;
}

void SimpleClient::setActive(bool active)
{
    if(active){
        client_socket_->abort();
        bool success=client_socket_->bind(QHostAddress(address_));
        if(!success)
            qDebug()<<"bind success"<<success<<"connecting:"<<server_url_<<":"<<port_;
        client_socket_->connectToHost(server_url_,port_);

    }
    else{
        client_socket_->disconnectFromHost();
    }
}

void SimpleClient::sendTextMessage(QString msg)
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

void SimpleClient::slotSendObject(QJsonObject obj)
{
    QJsonDocument doc(obj);
    sendTextMessage(QString::fromUtf8(doc.toJson()));
}

void SimpleClient::setAddress(QString address)
{
    address_=address;
}

void SimpleClient::slotReceiveMsg()
{
    QByteArray curData=client_socket_->readAll();
    QString curStringBack=(QString(curData.right(16)));
    msg_array_.append(curData);
    //qDebug()<<curStringBack;
    if(curStringBack.endsWith("\"}")||curStringBack.endsWith("\"\n}\n")){
        msg_=QString::fromUtf8(msg_array_);

       //qDebug()<<msg_<<msg_.size();
        emit receiveTextMsg(msg_);
        emit clientReceiveTextMsg(msg_);
        //qDebug()<<"finish receive msg len:"<<msg_.size();
        msg_.clear();
        msg_array_.clear();
    }

}

void SimpleClient::actualSendText()
{

    if(write_queue_.size()>0){
        client_socket_->write(write_queue_.front().toUtf8());
        client_socket_->waitForBytesWritten();
        write_queue_.pop_front();
    }
    else {
        status_=0;
        check_status_timer_->stop();
    }
}
