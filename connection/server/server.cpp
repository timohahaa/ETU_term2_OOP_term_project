#include "connection/server/Server.h"
//#include <QJsonDocument>
//#include <QJsonObject>
#include <QCoreApplication>
#include "connection/message.h"
#include <QNetworkInterface>

Server::Server(QObject *parent)
{
    this->setParent(parent);
    player1 = nullptr;
    player2 = nullptr;
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(process_connection()));

};

bool Server::start_listening()
{
    if(!tcpServer->listen(QHostAddress::LocalHost, 6000)){
        return false;
    }
    return true;
}

bool Server::is_listening()
{
    return tcpServer->isListening();
}

QString Server::get_address()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QString ip4String;

    for(int nIter=0; nIter<list.count(); nIter++)
    {
        if(!list[nIter].isLoopback())
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ){
                ip4String = list[nIter].toString();
                qDebug() << ip4String;
            }
    }


    return ip4String+":6000";
}

void Server::shutdown()
{
    auto msg = ServerMessages::EventOpponentLeft();
    if (player1){
        msg.send_to(player1);
    }
    if (player2){
        msg.send_to(player2);
    }
    tcpServer->close();

}

void Server::process_connection()
{
    qDebug()<<"Поступает соединение";
    if (player1 == nullptr){
        player1 = tcpServer->nextPendingConnection();
        connect(player1, SIGNAL(disconnected()), this, SLOT(player_left()));

        auto msg = ServerMessages::ConnectAccept();
        msg.send_to(player1);
        qDebug()<<"Есть игрок 1";
    }
    else if (player2 == nullptr){
        player2 =  tcpServer->nextPendingConnection();
        connect(player2, SIGNAL(disconnected()), this, SLOT(player_left()));

        auto msg = ServerMessages::ConnectAccept();
        msg.send_to(player2);

        qDebug()<<"Сервер: Есть игрок 2. Засылаю ивент начала";

        msg = ServerMessages::EventStartingUp();
        msg.send_to(player1);
        msg.send_to(player2);

    }
    else{

        auto other = tcpServer->nextPendingConnection();
        auto msg = ServerMessages::ConnectDecline();
        msg.send_to(other);
        //other->close();
        qDebug()<<"Игроков больше не принимаем";
    }
}

void Server::readyRead()
{

}

void Server::player_left()
{
    auto who_left = qobject_cast<QTcpSocket*>(QObject::sender());
    if (who_left == player1){
        if (player2){
            qDebug()<<"СЕРВЕР: Игрок 1 вышел";
            auto msg = ServerMessages::EventOpponentLeft();
            msg.send_to(player2);
        }
    }
    else if (who_left == player2){
        if (player1){
            qDebug()<<"СЕРВЕР: Игрок 2 вышел";
            auto msg = ServerMessages::EventOpponentLeft();
            msg.send_to(player1);
        }
    }
}
