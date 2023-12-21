
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
        method_handler(msg);

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
    else if (json["event"].toString() == "opponent_ready"){
        emit opponent_ready();
    }
    else if (json["event"].toString() == "game_started"){
        emit game_started(json["field_size"].toInt(),
                          json["numbers"].toInt(),
                          json["turns"].toInt());
    }
    else if (json["event"].toString() == "next_turn"){
        emit next_turn(
            json["scores1"].toInt(),
            json["scores2"].toInt(),
            json["turns"].toInt(),
            json["now_turn"].toBool());
    }
    else if (json["event"].toString() == "opponent_opened_cell"){
        emit opponent_opened_cell(
            json["i"].toInt(),
            json["j"].toInt()
        );
    }

    else if (json["event"].toString() == "win"){
        ServerMessages::EndStates state;
        switch (json["who"].toInt()){
        case 0:
            state = ServerMessages::Win;
            break;
        case 1:
            state = ServerMessages::Lose;
            break;
        default:
            state = ServerMessages::Draw;
            break;
        }
        emit game_over(state,
                       json["scores1"].toInt(),
                       json["scores2"].toInt()
                       );

    }

}

void QGameClient::method_handler(Message msg)
{
    auto json = msg.get_data().object();
    if (json["method"].toString() == "connect"){
        emit handshaking();
        handshake(msg);
    }
    if (json["method"].toString() == "set_field"){
        SetFieldResult res;
        bool ok = json["status"].toString() == "ok";
        if (ok){
            emit field_set_success(res);
            return;
        }
        QJsonObject extra = json["extra"].toObject();

        res.field_size_err = extra["field_size_error"].toBool();
        res.extra_digits = extra["extra_digits_error"].toBool();;
        res.not_enough_digits = extra["not_enough_digits_error"].toBool();;
        res.neighbours = extra["neighbours_error"].toBool();;

        QJsonArray errors = extra["errors"].toArray();

        for (auto err: errors) {
            QJsonArray coordinate = err.toArray();

            auto c = SetFieldResult::coord(coordinate[0].toInt(),
                                           coordinate[1].toInt());
            res.errors.append(c);
        }
        emit field_set_error(res);
    }
    if (json["method"].toString() == "open_cell"){
        emit opened_cell(
            json["i"].toInt(),
            json["j"].toInt(),
            json["val"].toInt()
        );
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
