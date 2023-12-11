
#include "qgameclient.h"
#include "../message.h"

QGameClient::QGameClient(QObject *parent)
    : QObject{parent}
{

    connect(this->socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(connection_error()));
    connect(this->socket, SIGNAL(connected()), this, SLOT(connection_success()));
    connect(this->socket, SIGNAL(readyRead()), this, SLOT(sockReadyRead()));
}

void QGameClient::connectToHost(QString host, quint16 port)
{
    socket->connectToHost(host, port);


}

void QGameClient::disconnect()
{
    socket->disconnectFromHost();
}


void QGameClient::connection_error()
{
    qDebug() << "ОШИБКА СОЕДИНЕНИЯ";
    emit cant_connect();
}

void QGameClient::connection_success()
{
    qDebug() << "СОЕДИНЕНИЕ УСПЕШНО";
    emit start_game();
}

void QGameClient::sockReadyRead()
{
     auto msg = Message::getFromSocket(this->socket);

    qDebug() << "КЛИЕНТ: ПРИШЛО СООБЩЕНИЕ" << msg.get_json_string();

}
