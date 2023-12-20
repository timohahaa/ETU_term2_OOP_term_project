#include "connection/server/Server.h"
//#include <QJsonDocument>
//#include <QJsonObject>
#include <QCoreApplication>
#include <QNetworkInterface>
#include "../message.h"

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
        connect(player1, SIGNAL(readyRead()), this, SLOT(readyRead()));

        auto msg = ServerMessages::ConnectAccept();
        msg.send_to(player1);
        qDebug()<<"Есть игрок 1";
    }
    else if (player2 == nullptr){
        player2 =  tcpServer->nextPendingConnection();
        connect(player2, SIGNAL(disconnected()), this, SLOT(player_left()));
        connect(player2, SIGNAL(readyRead()), this, SLOT(readyRead()));

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
    auto player = qobject_cast<QTcpSocket*>(QObject::sender());
    auto msg = Message::getFromSocket(player);
    auto json = msg.get_data().object();

    if (json.contains("method")){
            qDebug() << "СЕРВЕР: ПРИШЕЛ ЗАПРОС" << msg.get_json_string();
                                               if (json["method"].toString() == "timestamps"){
            quint64 time = json["time"].toVariant().toULongLong();
            if (player == player1){
                p1_time = time;
                qDebug()<<"Игрок 1 время:"<<p1_time;
            }
            else if (player == player2){
                p2_time = time;
                qDebug()<<"Игрок 2 время:"<<p2_time;
            }
            if (p1_time and p2_time){
                auto msg = ServerMessages::EventGameStarted();
                msg.send_to(player1);
                msg.send_to(player2);
            }
        }

    }

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
