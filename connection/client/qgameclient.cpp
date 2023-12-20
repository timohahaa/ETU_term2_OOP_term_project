
#include "qgameclient.h"
//#include "../message.h"

QGameClient::QGameClient(QObject *parent)
    : QObject{parent}
{

    connect(this->socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(connection_error()));
    connect(this->socket, SIGNAL(connected()), this, SLOT(connection_success()));
    connect(this->socket, SIGNAL(readyRead()), this, SLOT(sockReadyRead()));
}

void QGameClient::connectToHost(QString host, quint16 port)
{
    emit connecting();
    socket->connectToHost(host, port);

}

void QGameClient::disconnect()
{
    socket->disconnectFromHost();
}

void QGameClient::send(Message msg)
{
    msg.send_to(this->socket);
}

void QGameClient::process_answer(Message msg)
{
    auto json = msg.get_data().object();
    if (json.contains("method")){
        qDebug() << "КЛИЕНТ: ПРИШЕЛ ОТВЕТ НА МЕТОД" << msg.get_json_string();
        if (json["method"].toString() == "connect"){
            emit handshaking();
            handshake(msg);
        }
    }
    if (json.contains("event")){
        qDebug() << "КЛИЕНТ: НОВОЕ СОБЫТИЕ" << msg.get_json_string();
        event_handler(msg);
    }



}

void QGameClient::handshake(Message msg)
{
    auto json = msg.get_data().object();
    if (json["status"].toString() == "error"){
        if (json["reason"].toString() == "party_full"){
            emit party_full();
            socket->disconnect();
        }
    }
    if (json["status"].toString() == "success"){
        emit connected();
    }


}

void QGameClient::event_handler(Message msg)
{
    auto json = msg.get_data().object();
    if (json["event"].toString() == "opponent_left"){
        emit opponent_left();
    }
    else if (json["event"].toString() == "starting_up"){
        emit starting_game();
    }
    else if (json["event"].toString() == "game_started"){
        emit game_started();
    }

}

void QGameClient::connection_error()
{
    qDebug() << "КЛИЕНТ: ОШИБКА СОЕДИНЕНИЯ. СЕРВЕР НЕ НАЙДЕН.";
    emit connect_failed();
}

void QGameClient::connection_success()
{
    qDebug() << "КЛИЕНТ: СОЕДИНЕНИЕ УСПЕШНО. Делаю рукопожатие.";
    emit connecting();
}

void QGameClient::sockReadyRead()
{
    auto msg = Message::getFromSocket(this->socket);
    process_answer(msg);

}
